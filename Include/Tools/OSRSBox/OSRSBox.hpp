#ifndef OSRSBOX_HPP_INCLUDED
#define OSRSBOX_HPP_INCLUDED

#include <string>
#include "../json.hpp"
#include "../sqlite/sqlite_orm.h"

namespace OSRSBox
{
    const std::string DB_HOST = "osrsbox.com";
    const std::string DB_URI  = "/osrsbox-db/";
    const std::string DB_URI_ITEMS_STATIC         = DB_URI + "items-json/"; // + ItemID + .json eg std::to_string(ItemID) + ".json"
    const std::string DB_URI_ITEMS_SEARCH         = DB_URI + "items-search.json";
    const std::string DB_URI_ITEMS_COMPLETE       = DB_URI + "items-complete.json";
    const std::string DB_URI_ITEMS_SUMMARY        = DB_URI + "items-summary.json";
    const std::string DB_URI_MONSTERS_COMPLETE    = DB_URI + "monsters-complete.json";
    const std::string DB_URI_PRAYERS_COMPLETE     = DB_URI + "prayers-complete.json";

    namespace Remote
    {
        std::string GetLatestVersion();
        nlohmann::json Get(const std::string& URI, bool HTTPS = true, const nlohmann::json::parser_callback_t Callback = nullptr);
    }

    namespace Database
    {
        namespace detail
        {
            struct Info
            {
                int id;
                std::string version;
                std::uint32_t itemRows;
            };

            struct Item
            {
                int id;
                std::string name;
                bool duplicate;
                std::string json;
            };

            struct Prayer
            {
                int id;
                std::string name;
                std::string json;
            };

            inline auto InitStorage(const std::string &Path)
            {
                return sqlite_orm::make_storage(Path,
                                                sqlite_orm::make_unique_index("name", &Prayer::name),
                                                sqlite_orm::make_table("osrsbox",
                                                                       sqlite_orm::make_column("id", &Info::id, sqlite_orm::primary_key()),
                                                                       sqlite_orm::make_column("version", &Info::version),
                                                                       sqlite_orm::make_column("itemRows", &Info::itemRows)),
                                                sqlite_orm::make_table("items-complete",
                                                                       sqlite_orm::make_column("id", &Item::id, sqlite_orm::primary_key()),
                                                                       sqlite_orm::make_column("name", &Item::name),
                                                                       sqlite_orm::make_column("duplicate", &Item::duplicate),
                                                                       sqlite_orm::make_column("json", &Item::json)),
                                               sqlite_orm::make_table("prayers-complete",
                                                                       sqlite_orm::make_column("id", &Prayer::id, sqlite_orm::primary_key()),
                                                                       sqlite_orm::make_column("name", &Prayer::name),
                                                                       sqlite_orm::make_column("json", &Prayer::json))
                );
            }
            using Storage = decltype(InitStorage(""));

            inline std::string Path;
            inline std::string CompactPath;
            inline std::string Version;
            inline std::unique_ptr<Storage> Database;

            bool IsPopulated(); // returns true if the database is populated and up-to-date
            bool Populate(); // Populates the OSRSBox database from the remote OSRSBox json-database
        }

        bool Init();
    }
}

#endif // OSRSBOX_HPP_INCLUDED