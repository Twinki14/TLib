#ifndef OSRSBOX_ITEMS_HPP_INCLUDED
#define OSRSBOX_ITEMS_HPP_INCLUDED

#include <map>
#include <cstdint>
#include <shared_mutex>
#include "schemas/Item.hpp"

namespace OSRSBox::Items
{
    namespace cache
    {
        inline std::mutex Lock;
        inline std::map<std::uint32_t, OSRSBox::Items::Item> Items;
        void Add(std::int32_t ID, std::string_view rawJson);
        void Add(std::int32_t ID, OSRSBox::Items::Item& Item);
    }

    std::string Find(std::int32_t ID, bool Cache = true);
    std::int32_t Find(std::string_view Name, bool Fuzzy = false, bool AllowDuplicates = false, bool Cache = true);
    std::vector<std::int32_t> FindAll(std::string_view Name, bool Fuzzy = false, bool AllowDuplicates = false, bool Cache = true);

    Item Get(std::int32_t ID);
    Item Get(std::string_view Name, bool Fuzzy = false);
}

#endif // OSRSBOX_ITEMS_HPP_INCLUDED