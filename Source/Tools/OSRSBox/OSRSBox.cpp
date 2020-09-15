#include "../../Include/Tools/OSRSBox/OSRSBox.hpp"
#include "../../Include/TScript.hpp"
#include "../../Include/Tools/WinnHttpComm.hpp"
#include <Core/Types/Countdown.hpp>
#include <shlobj.h>

namespace OSRSBox
{
    namespace Remote
    {
        // Fetch latest version of OSRSBox from https://release-monitoring.org
        std::string GetLatestVersion()
        {
            static std::string Version;
            if (!Version.empty())
                return Version;

            constexpr uint32_t ProjectID = 32210;

            WinnHttpComm::Request Req;
            Req.Host = "release-monitoring.org";
            Req.Method = "GET";
            Req.URI = "/api/project/" + std::to_string(ProjectID);
            Req.HTTPS = true;

            DebugLog("Fetching latest OSRSBox version");
            const auto Response = WinnHttpComm::Handle(Req);

            try
            {
                nlohmann::json json = nlohmann::json::parse(Response.Content);

                if (json.empty() || json.is_null() || !json.contains("name") || !json.contains("version") || json.at("name").get<std::string>() != "osrsbox")
                {
                    Version.clear();
                    return "";
                }

                Version = std::string(json.at("version").get<std::string>());
                DebugLog("Latest > {}", Version);
            }
            catch (nlohmann::json::parse_error& e)
            {
                DebugLog("Parse error > {}", e.what());
            }
            catch (nlohmann::json::exception& e)
            {
                DebugLog("JSON exception > {}", e.what());
            }

            return Version;
        }

        nlohmann::json Get(const std::string& URI, bool HTTPS, const nlohmann::json::parser_callback_t Callback)
        {
            WinnHttpComm::Request Req;
            Req.Host = DB_HOST;
            Req.Method = "GET";
            Req.URI = URI;
            Req.HTTPS = HTTPS;
            const auto Response = WinnHttpComm::Handle(Req);

            if (Response.Status != 200)
                return nlohmann::json();

            try
            {
                nlohmann::json json = nlohmann::json::parse(Response.Content, Callback);
                return json;
            }
            catch (nlohmann::json::parse_error& e)
            {
                DebugLog("Parse error > {}", e.what());
            }
            catch (nlohmann::json::exception& e)
            {
                DebugLog("JSON exception > {}", e.what());
            }
            return nlohmann::json();
        }
    }

    namespace Database
    {
        namespace detail
        {
            bool IsPopulated()
            {
                if (!Database) return false;

                if (!Database->table_exists("osrsbox") || !Database->table_exists("items-complete"))
                    return false;

                std::uint32_t ExpectedItemRows = 0;
                std::string DatabaseVersion;
                try
                {
                    auto InfoRow = Database->get<Info>(0);
                    ExpectedItemRows = InfoRow.itemRows;
                    DatabaseVersion = InfoRow.version;
                } catch (std::system_error& e)
                {
                    DebugLog("{} > OSRSBox database information missing - {}", CompactPath, e.what());
                    return false;
                }

                int DatabaseItemRows = Database->count<Item>();
                if (DatabaseItemRows <= 0 || DatabaseItemRows != ExpectedItemRows)
                {
                    DebugLog("{} > items-complete not correctly populated", CompactPath);
                    return false;
                }

                std::string LatestVersion = OSRSBox::Remote::GetLatestVersion();
                if (DatabaseVersion.empty() || DatabaseVersion != OSRSBox::Remote::GetLatestVersion())
                {
                    DebugLog("{} > Version mismatch", CompactPath);
                    return false;
                }

                DebugLog("{} > Populated and up-to-date", CompactPath);
                return true;
            }

            bool Populate()
            {
                if (!Database) return false;
                if (IsPopulated()) return true;

                int Attempts = 0;
                constexpr int MaxAttempts = 10;
                Database->busy_timeout(5000);

                POPULATE:
                try
                {
                    auto guard = Database->transaction_guard(); // turn off auto-comitting
                    Database->replace<Info>( { 0, "0.0.0", 0}); // set the db to RESERVED - https://sqlite.org/lockingv3.html - replace uses INSERT and DELETE

                    Timer GlobalMark;
                    DebugLog("{} > Populating database", CompactPath);

                    Timer Mark;
                    Info DatabaseInfo = { 0, OSRSBox::Remote::GetLatestVersion(), 0 };
                    if (DatabaseInfo.version.empty())
                    {
                        DebugLog("{} > Failed to fetch latest OSRSBox version > Took {}", CompactPath, MillisToHumanMedium(Mark.GetTimeElapsed()));
                        guard.commit(); // turn on auto-comitting
                        return false;
                    }
                    Database->replace<Info>(DatabaseInfo);

                    Mark.Restart();
                    DebugLog("{} > Fetching items-complete.json, this may take a bit", CompactPath);
                    nlohmann::json json = OSRSBox::Remote::Get(OSRSBox::DB_URI_ITEMS_COMPLETE, true);
                    if (json.empty() || json.is_null())
                    {
                        DebugLog("{} > Failed to fetch items-complete.json, the json is empty or null > Took {}", CompactPath, MillisToHumanMedium(Mark.GetTimeElapsed()));
                        guard.commit(); // turn on auto-comitting
                        return false;
                    }
                    DebugLog("{} > Fetched and parsed items-complete.json > ItemRows {} > Took {}", CompactPath, json.size(), MillisToHumanMedium(Mark.GetTimeElapsed()));

                    DatabaseInfo = { 0, DatabaseInfo.version, json.size() };
                    Database->replace<Info>(DatabaseInfo);
                    Database->remove_all<Item>();

                    auto Iterator = json.begin();
                    while (Iterator != json.end())
                    {
                        Item ItemEntry = { Iterator.value().at("id").get<std::int32_t>(),
                                           Iterator.value().at("name").get<std::string>(),
                                           Iterator.value().at("duplicate").get<bool>(),
                                           Iterator.value().dump() };
                        Database->replace<Item>(ItemEntry);
                        Iterator = json.erase(Iterator);
                    }

                    guard.commit(); // turn on auto-comitting, explict because why not
                    DebugLog("{} > Populated database > Took {}", CompactPath, MillisToHumanMedium(GlobalMark.GetTimeElapsed()));
                } catch (std::system_error& e)
                {
                    if (e.code().value() == SQLITE_BUSY)
                    {
                        DebugLog("{} > Database busy - Sleeping for 10s - x{}", CompactPath, Attempts);
                        Wait(10000);
                        if (IsPopulated())
                        {
                            DebugLog("{} > Populated by another thread", CompactPath);
                            return true;
                        }

                        if (Attempts >= MaxAttempts)
                        {
                            DebugLog("{} > Too many attempts trying to populate, somethings wrong", CompactPath);
                            Script::Stop("Database failure");
                            Database.reset();
                            return false;
                        }
                        Attempts++;

                        if (IsPopulated())
                        {
                            DebugLog("{} > Populated by another thread", CompactPath);
                            return true;
                        }

                        goto POPULATE;
                    } else
                    {
                        DebugLog("{} > Caught > {} - {}", CompactPath, e.what(), e.code().value());
                    }
                    return false;
                } catch (...)
                {
                    DebugLog("Database failed > Unknown exception");
                    return false;
                }
                return true;
            }
        }

        bool Init()
        {
            if (detail::Database) return true;

            if (detail::Path.empty())
            {
                char TempPath[MAX_PATH];
                if (SHGetFolderPath(nullptr, CSIDL_PROFILE, nullptr, 0, TempPath) == S_OK)
                {
                    detail::Path = TempPath;
                    assert(!detail::Path.empty());
                    detail::Path += R"(\AlpacaBot\Cache\osrsbox.sqlite)";
                }
                detail::CompactPath = R"(AlpacaBot/Cache/osrsbox.sqlite)";
                DebugLog("Database path > {} Compact > {}", detail::Path, detail::CompactPath);
            }
            if (detail::Path.empty()) return false;

            try
            {
                detail::Database = std::make_unique<detail::Storage>(detail::InitStorage(detail::Path));
            } catch (const std::system_error& e)
            {
                DebugLog("Database failed > {}", e.what());
                detail::Database.reset();
                return false;
            } catch (...)
            {
                DebugLog("Database failed > Unknown exception");
                detail::Database.reset();
                return false;
            }
            detail::Database->sync_schema();

            if (!detail::Populate())
            {
                detail::Database.reset();
                return false;
            }

            DebugLog("{} > Initiated", detail::CompactPath);
            return true;
        }
    }
}