#include <list>
#include "../../../Include/TScript.hpp"
#include "WinnHttpComm.hpp"
#include "OSRSBox/OSRSBox.hpp"
#include "OSRSBox/Items.hpp"
#include "json.hpp"

using namespace OSRSBox::Database;

namespace OSRSBox::Items
{
    namespace cache
    {
        void Add(std::int32_t ID, std::string_view rawJson)
        {
            if (ID < 0) return;
            if (!Items.count((uint32_t) ID) && !rawJson.empty())
            {
                OSRSBox::Items::Item Item;
                nlohmann::json json;
                try
                {
                    json = nlohmann::json::parse(rawJson);
                    nlohmann::from_json(json, Item);
                } catch (...)
                {
                    return;
                }
                DebugLog("OSRSBox::Items > Caching > {} - {}", Item.id, Item.name);
                std::lock_guard<std::mutex> guard(cache::Lock);
                Items[ID] = std::move(Item);
            }
        }

        void Add(std::int32_t ID, OSRSBox::Items::Item& Item)
        {
            if (ID < 0) return;
            if (!Items.count((uint32_t) ID))
            {
                std::lock_guard<std::mutex> guard(cache::Lock);
                DebugLog("OSRSBox::Items > Caching > {} - {}", Item.id, Item.name);
                Items[ID] = std::move(Item);
            }
        }
    }

    std::string Find(std::int32_t ID, bool Cache)
    {
        if (OSRSBox::Items::cache::Items.count(ID))
        {
            std::lock_guard<std::mutex> guard(cache::Lock);
            const OSRSBox::Items::Item& I = OSRSBox::Items::cache::Items.at(ID);
            return I.name;
        }

        if (!OSRSBox::Database::Init())
            return "";


        auto Rows = detail::Database->get_all<detail::Item>(sqlite_orm::where(sqlite_orm::is_equal(&detail::Item::id, ID)));
        if (!Rows.empty())
        {
            if (Cache) cache::Add(Rows.front().id, Rows.front().json);
            return Rows.front().name;
        }
        return "";
    }

    std::int32_t Find(std::string_view Name, bool Fuzzy, bool AllowDuplicates, bool Cache)
    {
        cache::Lock.lock();
        auto FindInCached = [&](const std::pair<std::uint32_t, OSRSBox::Items::Item>& p) -> bool
        {
            return (AllowDuplicates || !p.second.duplicate)
                   && (Fuzzy ? p.second.name.find(Name) != std::string::npos : p.second.name == Name);
        };
        auto InCache = std::find_if(cache::Items.begin(), cache::Items.end(), FindInCached);
        std::int32_t ID = InCache != cache::Items.end() ? InCache->second.id : -1;
        cache::Lock.unlock();

        if (ID >= 0) return ID;

        if (!OSRSBox::Database::Init())
            return -1;

        std::string NameSql(Name);
        if (Fuzzy)
        {
            NameSql = "%" + NameSql + "%";
            for (auto& Item: detail::Database->iterate<detail::Item>(sqlite_orm::where(sqlite_orm::like(&detail::Item::name, NameSql))))
            {
                if (AllowDuplicates || !Item.duplicate)
                {
                    if (Cache) cache::Add(Item.id, Item.json);
                    return Item.id;
                }
            }
        } else
        {
            for (auto& Item: detail::Database->iterate<detail::Item>(sqlite_orm::where(sqlite_orm::is_equal(&detail::Item::name, NameSql))))
            {
                if (AllowDuplicates || !Item.duplicate)
                {
                    if (Cache) cache::Add(Item.id, Item.json);
                    return Item.id;
                }
            }
        }
        return -1;
    }

    std::vector<std::int32_t> FindAll(std::string_view Name, bool Fuzzy, bool AllowDuplicates, bool Cache)
    {
        std::vector<std::int32_t> Result;
        cache::Lock.lock();
        auto FindInCached = [&](const std::pair<std::uint32_t, OSRSBox::Items::Item>& p) -> bool
        {
            return (AllowDuplicates || !p.second.duplicate)
                   && (Fuzzy ? p.second.name.find(Name) != std::string::npos : p.second.name == Name);
        };
        auto Iterator = cache::Items.begin(), end = cache::Items.end();
        while (Iterator != end)
        {
            Iterator = std::find_if(Iterator, end, FindInCached);
            if (Iterator != end) Result.emplace_back(Iterator->second.id);
        }
        cache::Lock.unlock();

        if (!OSRSBox::Database::Init())
            return Result;

        std::string NameSql(Name);
        if (Fuzzy)
        {
            NameSql = "%" + NameSql + "%";
            for (auto& Item: detail::Database->iterate<detail::Item>(sqlite_orm::where(sqlite_orm::like(&detail::Item::name, NameSql))))
            {
                if (AllowDuplicates || !Item.duplicate)
                {
                    if (Cache) cache::Add(Item.id, Item.json);
                    Result.emplace_back(Item.id);
                }
            }
        } else
        {
            for (auto& Item: detail::Database->iterate<detail::Item>(sqlite_orm::where(sqlite_orm::is_equal(&detail::Item::name, NameSql))))
            {
                if (AllowDuplicates || !Item.duplicate)
                {
                    if (Cache) cache::Add(Item.id, Item.json);
                    Result.emplace_back(Item.id);
                }
            }
        }
        return Result;
    }

    OSRSBox::Items::Item Get(std::int32_t ID)
    {
        std::string FoundName = Find(ID, true);
        if (!FoundName.empty())
        {
            std::lock_guard<std::mutex> guard(cache::Lock);
            return OSRSBox::Items::Item(OSRSBox::Items::cache::Items.at(ID));
        }
        return OSRSBox::Items::Item();
    }

    OSRSBox::Items::Item Get(std::string_view Name, bool Fuzzy)
    {
        std::int32_t FoundID = Find(Name, Fuzzy, false, true);
        if (FoundID >= 0)
        {
            std::lock_guard<std::mutex> guard(cache::Lock);
            return OSRSBox::Items::Item(OSRSBox::Items::cache::Items.at(FoundID));
        }
        return OSRSBox::Items::Item();
    }
}
