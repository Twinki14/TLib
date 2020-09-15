#ifndef TLIB_CONTAINERS_HPP_INCLUDED
#define TLIB_CONTAINERS_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <vector>
#include <Game/Interactable/Item.hpp>

/** TODO
 * - Switch ItemAmounts to uint32_t instead of int32_t - no reason to have it -1, it shouldn't matter if the item in the equivalent index is -1
 * - ContainsOnly(... array)
 * - GetIndicesOf(... array)
 * - Update/Refresh(), reconstruct the container, consider a mutex or something to make it thread-safe
 */

namespace Containers
{
    enum Source
    {
        INVENTORY,
        EQUIPMENT,
        BANK,
        ITEM_CONTAINER // non-specific, used for any Internal::ItemContainer objects
    };

    class Item
    {
    public:
        Item() = default;
        Item(std::int32_t ID, std::string_view Name, std::uint32_t Amount, std::int32_t Index, std::int32_t Source);

        std::int32_t GetID()      const  { return ID; };
        std::string GetName()     const  { return Name; };
        std::uint32_t GetAmount() const  { return Amount; };
        std::int32_t GetIndex()   const  { return Index; };
        std::int32_t GetSource()  const  { return Source; };

        explicit operator bool() const;
        bool operator==(const Item& I) const; /// @returns True if the ID, Name, Index, and Source are equal
        bool operator!=(const Item& I) const; /// @returns True if the ID, Name, Index, or Source are different

        ~Item() = default;
    private:
        std::int32_t ID = -1;
        std::string Name;
        std::uint32_t Amount = 0;
        std::int32_t Index = -1;
        std::int32_t Source = -1;
    };

    class Container
    {
    public:
        Container() = default;
        Container(std::int32_t Source);
        Container(std::vector<Item> Items, std::int32_t Source);
        Container(std::vector<std::int32_t> ItemIDs,
                  std::vector<std::string> ItemNames,
                  std::vector<std::int32_t> ItemAmounts,
                  std::int32_t Source);

        std::uint32_t Count(std::int32_t ID) const;
        std::uint32_t Count(std::string_view Name) const;
        std::uint32_t Count(const Interactable::Item& Item) const;
        std::uint32_t Count(const std::vector<std::int32_t>& IDs) const;
        std::uint32_t Count(const std::vector<std::string>& Names) const;
        std::uint32_t Count(const std::vector<Interactable::Item>& Items) const;

        std::vector<std::uint32_t> CountIndividual(const std::vector<std::int32_t>& IDs) const;
        std::vector<std::uint32_t> CountIndividual(const std::vector<std::string>& Names) const;
        std::vector<std::uint32_t> CountIndividual(const std::vector<Interactable::Item>& Items) const;

        std::uint32_t CountEmpty() const;
        std::uint32_t CountOccupied() const;

        std::int32_t GetIndexOf(std::int32_t ID) const;
        std::int32_t GetIndexOf(std::string_view Name) const;
        std::int32_t GetAnyIndexOf(const std::vector<std::int32_t>& IDs) const; // Returns first found ID index
        std::int32_t GetAnyIndexOf(const std::vector<std::string>& Names) const; // Returns first found Name Index
        std::int32_t GetAnyIndexOf(const std::vector<Interactable::Item>& Items) const; // Returns first found index that's equal to the Item ID

        std::vector<std::uint32_t> GetIndicesOf(std::int32_t ID) const;
        std::vector<std::uint32_t> GetIndicesOf(std::string_view Name) const;
        std::vector<std::int32_t> GetIndicesOf(const std::vector<std::int32_t>& IDs) const; // in order to ids
        std::vector<std::int32_t> GetIndicesOf(const std::vector<std::string>& Names) const;
        std::vector<std::int32_t> GetIndicesOf(const std::vector<Interactable::Item>& Items) const;
        
        bool Contains(std::int32_t ID) const;
        bool Contains(std::string_view Name) const;
        bool Contains(const Interactable::Item& Item) const;
        bool Contains(const std::vector<std::int32_t>& IDs) const; // true if all items are found at least once
        bool Contains(const std::vector<std::string>& Names) const; // true if all items are found at least once
        bool Contains(const std::vector<Interactable::Item>& Items) const;

        bool ContainsAny(const std::vector<std::int32_t>& IDs) const;
        bool ContainsAny(const std::vector<std::string>& Names) const;
        bool ContainsAny(const std::vector<Interactable::Item>& Items) const;

        bool ContainsOnly(std::int32_t ID) const; // ignores non-items/empty slots (-1)
        bool ContainsOnly(std::string_view Name) const;
        bool ContainsOnly(const Interactable::Item& Item) const;
        bool ContainsOnly(const std::vector<std::int32_t>& IDs) const;
        bool ContainsOnly(const std::vector<std::string>& Names) const;
        bool ContainsOnly(const std::vector<Interactable::Item>& Items) const;

        bool IsEmpty() const;

        const std::vector<std::int32_t>& GetItemIDs() const;
        const std::vector<std::string>& GetItemNames() const;
        const std::vector<std::int32_t>& GetItemAmounts() const;
        const std::vector<Item>& GetItems() const;

        explicit operator bool() const; /// returns true if there's at least one item
        bool operator==(const Container& I) const; /// @returns True if the ID, Name, Index, and Source are equal
        bool operator!=(const Container& I) const; /// @returns True if the ID, Name, Index, or Source are different

        ~Container() = default;
    private:
        std::vector<std::int32_t> ItemIDs;
        std::vector<std::string> ItemNames;
        std::vector<std::int32_t> ItemAmounts;
        std::vector<Containers::Item> Items;
        std::int32_t Source = -1;
    };
}




#endif // TLIB_CONTAINERS_HPP_INCLUDED