#include "../../Include/Utilities/Containers.hpp"
#include <numeric>
#include <Game/Interfaces/GameTabs/Inventory.hpp>
#include <Game/Interfaces/GameTabs/Equipment.hpp>
#include <Game/Interfaces/Bank.hpp>

namespace Containers
{
    Item::Item(std::int32_t ID, std::string_view Name, std::uint32_t Amount, std::int32_t Index, std::int32_t Source)
        : ID(ID), Name(Name), Amount(Amount < 0 ? 0 : Amount), Index(Index), Source(Source)
    {

    }

    Item::operator bool() const
    {
        switch (Source)
        {
            case INVENTORY:
            case EQUIPMENT:
            case BANK: return this->ID >= 0 && !this->Name.empty() && this->Index >= 0;
            default: break;
        }
        return false;
    }

    bool Item::operator==(const Item& I) const
    {
        return this->ID == I.ID && this->Name == I.Name && this->Index == I.Index && this->Source == I.Source;
    }

    bool Item::operator!=(const Item& I) const
    {
        return this->ID != I.ID || this->Name != I.Name || this->Index != I.Index || this->Source != I.Source;
    }

    Container::Container(std::int32_t Source)
    {
        switch (Source)
        {
            case INVENTORY:
                *this = Container(Inventory::GetItemIDs(),
                                  Inventory::GetItemNames(),
                                  Inventory::GetItemAmounts(),
                                  Source);
                return;

            case EQUIPMENT:
                *this = Container(Equipment::GetItemIDs(),
                                  Equipment::GetItemNames(),
                                  Equipment::GetItemAmounts(),
                                  Source);
                return;

            case BANK:
                *this = Container(Bank::GetItemIDs(),
                                  Bank::GetItemNames(),
                                  Bank::GetItemAmounts(),
                                  Source);
                return;
            default: break;
        }
        this->Source = -1;
    }

    Container::Container(std::vector<Item> Items, std::int32_t Source) : Items(std::move(Items)), Source(Source)
    {

    }

    Container::Container(std::vector<std::int32_t> ItemIDs,
                         std::vector<std::string> ItemNames,
                         std::vector<std::int32_t> ItemAmounts,
                         std::int32_t Source)
    {
        switch (Source)
        {
            case INVENTORY:
            case EQUIPMENT:
            case BANK:

                if (ItemIDs.size() != ItemNames.size() || ItemIDs.size() != ItemAmounts.size())
                    break;

                this->Source = Source;
                this->ItemIDs = std::move(ItemIDs);
                this->ItemNames = std::move(ItemNames);
                this->ItemAmounts = std::move(ItemAmounts);
                for (std::uint32_t I = 0; I < this->ItemIDs.size(); I++)
                    this->Items.emplace_back(Containers::Item(this->ItemIDs[I], this->ItemNames[I], this->ItemAmounts[I], I, this->Source));
            return;
            default: break;
        }
        Source = -1;
    }

    std::uint32_t Container::Count(std::int32_t ID) const
    {
        const auto CountID = [&](std::uint32_t Sum, const Containers::Item& I)
        {
            if (I.GetID() == ID)
                return Sum + I.GetAmount();
            return Sum;
        };
        return std::accumulate(this->Items.begin(), this->Items.end(), 0, CountID); // std::reduce
    }

    std::uint32_t Container::Count(std::string_view Name) const
    {
        const auto CountName = [&](std::uint32_t Sum, const Containers::Item& I)
        {
            if (I.GetName() == Name)
                return Sum + I.GetAmount();
            return Sum;
        };
        return std::accumulate(this->Items.begin(), this->Items.end(), 0, CountName); // std::reduce
    }

    std::uint32_t Container::Count(const Interactable::Item& Item) const
    {
        const auto CountItem = [&](std::uint32_t Sum, const Containers::Item& I)
        {
            if (I.GetID() == Item.GetID())
                return Sum + I.GetAmount();
            return Sum;
        };
        return std::accumulate(this->Items.begin(), this->Items.end(), 0, CountItem); // std::reduce
    }

    std::uint32_t Container::Count(const std::vector<std::int32_t>& IDs) const
    {
        const auto CountIDs = [&](std::uint32_t Sum, const Containers::Item& I)
        {
            auto At = std::find(IDs.begin(), IDs.end(), I.GetID());
            if (At != IDs.end())
                return Sum + I.GetAmount();
            return Sum;
        };
        return std::accumulate(this->Items.begin(), this->Items.end(), 0, CountIDs); // std::reduce
    }

    std::uint32_t Container::Count(const std::vector<std::string>& Names) const
    {
        const auto CountNames = [&](std::uint32_t Sum, const Containers::Item& I)
        {
            auto At = std::find(Names.begin(), Names.end(), I.GetName());
            if (At != Names.end())
                return Sum + I.GetAmount();
            return Sum;
        };
        return std::accumulate(this->Items.begin(), this->Items.end(), 0, CountNames); // std::reduce
    }

    std::uint32_t Container::Count(const std::vector<Interactable::Item>& InteractableItems) const
    {
        const auto CountItems = [&](std::uint32_t Sum, const Containers::Item& I)
        {
            auto At = std::find_if(InteractableItems.begin(), InteractableItems.end(),
                                   [&](const Interactable::Item& Item) { return Item.GetID() == I.GetID(); });
            if (At != InteractableItems.end())
                return Sum + I.GetAmount();
            return Sum;
        };
        return std::accumulate(this->Items.begin(), this->Items.end(), 0, CountItems); // std::reduce
    }

    std::vector<std::uint32_t> Container::CountIndividual(const std::vector<std::int32_t>& IDs) const
    {
        std::vector<std::uint32_t> Result;
        std::for_each(IDs.begin(), IDs.end(), [&] (std::int32_t ID) { Result.emplace_back(this->Count(ID)); });
        return Result;
    }

    std::vector<std::uint32_t> Container::CountIndividual(const std::vector<std::string>& Names) const
    {
        std::vector<std::uint32_t> Result;
        std::for_each(Names.begin(), Names.end(), [&] (std::string_view Name) { Result.emplace_back(this->Count(Name)); });
        return Result;
    }

    std::vector<std::uint32_t> Container::CountIndividual(const std::vector<Interactable::Item>& InteractableItems) const
    {
        std::vector<std::uint32_t> Result;
        std::for_each(InteractableItems.begin(), InteractableItems.end(), [&] (const Interactable::Item& Item) { Result.emplace_back(this->Count(Item.GetID())); });
        return Result;
    }

    std::uint32_t Container::CountEmpty() const
    {
        return std::count(this->ItemIDs.begin(), this->ItemIDs.end(), -1);
    }

    std::uint32_t Container::CountOccupied() const
    {
        return this->Items.size() - this->CountEmpty();
    }

    std::int32_t Container::GetIndexOf(std::int32_t ID) const
    {
        auto At = std::find_if(this->Items.begin(), this->Items.end(), [&](const Containers::Item& I) { return I.GetID() == ID; });
        return At != this->Items.end() ? At->GetIndex() : -1;
    }

    std::int32_t Container::GetIndexOf(std::string_view Name) const
    {
        auto At = std::find_if(this->Items.begin(), this->Items.end(), [&](const Containers::Item& I) { return I.GetName() == Name; });
        return At != this->Items.end() ? At->GetIndex() : -1;
    }

    std::int32_t Container::GetAnyIndexOf(const std::vector<std::int32_t>& IDs) const
    {
        std::int32_t Index = -1;
        const auto FindIndex = [&](std::int32_t ID)
        {
            auto At = std::find_if(this->Items.begin(), this->Items.end(),
                                   [&](const Containers::Item& Item) { return Item.GetID() == ID; });
            Index = At != this->Items.end() ? At->GetIndex() : -1;
            return Index >= 0;
        };
        std::any_of(IDs.begin(), IDs.end(), FindIndex);
        return Index;
    }

    std::int32_t Container::GetAnyIndexOf(const std::vector<std::string>& Names) const
    {
        std::int32_t Index = -1;
        const auto FindIndex = [&](std::string_view Name)
        {
            auto At = std::find_if(this->Items.begin(), this->Items.end(),
                                [&](const Containers::Item& Item) { return Item.GetName() == Name; });
            Index = At != this->Items.end() ? At->GetIndex() : -1;
            return Index >= 0;
        };
        std::any_of(Names.begin(), Names.end(), FindIndex);
        return Index;
    }

    std::int32_t Container::GetAnyIndexOf(const std::vector<Interactable::Item>& InteractableItems) const
    {
        std::int32_t Index = -1;
        const auto FindIndex = [&](const Interactable::Item& Interactable)
        {
            auto At = std::find_if(this->Items.begin(), this->Items.end(),
                                [&](const Containers::Item& Item) { return Item.GetID() == Interactable.GetID(); });
            Index = At != this->Items.end() ? At->GetIndex() : -1;
            return Index >= 0;
        };
        std::any_of(InteractableItems.begin(), InteractableItems.end(), FindIndex);
        return Index;
    }

    std::vector<std::uint32_t> Container::GetIndicesOf(std::int32_t ID) const
    {
        std::vector<std::uint32_t> Result;
        std::for_each(this->Items.begin(), this->Items.end(), [&](const Containers::Item& I)
        {
            if (I.GetID() == ID) Result.emplace_back(I.GetIndex());
        });
        return Result;
    }

    std::vector<std::uint32_t> Container::GetIndicesOf(std::string_view Name) const
    {
        std::vector<std::uint32_t> Result;
        std::for_each(this->Items.begin(), this->Items.end(), [&](const Containers::Item& I)
        {
            if (I.GetName() == Name) Result.emplace_back(I.GetIndex());
        });
        return Result;
    }

    std::vector<std::int32_t> Container::GetIndicesOf(const std::vector<std::int32_t>& IDs) const
    {
        return std::vector<std::int32_t>();
    }

    std::vector<std::int32_t> Container::GetIndicesOf(const std::vector<std::string>& Names) const
    {
        return std::vector<std::int32_t>();
    }

    bool Container::Contains(std::int32_t ID) const
    {
        auto Found = std::find(this->ItemIDs.begin(), this->ItemIDs.end(), ID);
        return Found != this->ItemIDs.end();
    }

    bool Container::Contains(std::string_view Name) const
    {
        auto Found = std::find(this->ItemNames.begin(), this->ItemNames.end(), Name);
        return Found != this->ItemNames.end();
    }

    bool Container::Contains(const Interactable::Item& Item) const
    {
        auto Found = std::find(this->ItemIDs.begin(), this->ItemIDs.end(), Item.GetID());
        return Found != this->ItemIDs.end();
    }

    bool Container::Contains(const std::vector<std::int32_t>& IDs) const
    {
        return std::all_of(IDs.begin(), IDs.end(), [&](std::int32_t ID) { return this->Contains(ID); });
    }

    bool Container::Contains(const std::vector<std::string>& Names) const
    {
        return std::all_of(Names.begin(), Names.end(), [&](std::string_view Name) { return this->Contains(Name); });
    }

    bool Container::Contains(const std::vector<Interactable::Item>& InteractableItems) const
    {
        return std::all_of(InteractableItems.begin(), InteractableItems.end(), [&](const Interactable::Item& Item) { return this->Contains(Item); });
    }

    bool Container::ContainsAny(const std::vector<std::int32_t>& IDs) const
    {
        return std::any_of(IDs.begin(), IDs.end(), [&](std::int32_t ID) { return this->Contains(ID); });
    }

    bool Container::ContainsAny(const std::vector<std::string>& Names) const
    {
        return std::any_of(Names.begin(), Names.end(), [&](std::string_view Name) { return this->Contains(Name); });
    }

    bool Container::ContainsAny(const std::vector<Interactable::Item>& InteractableItems) const
    {
        return std::any_of(InteractableItems.begin(), InteractableItems.end(), [&](const Interactable::Item& Item) { return this->Contains(Item); });
    }

    bool Container::ContainsOnly(std::int32_t ID) const
    {
        return std::all_of(this->ItemIDs.begin(), this->ItemIDs.end(), [&](std::int32_t ItemID) { return ItemID == -1 || ItemID == ID; });
    }

    bool Container::ContainsOnly(std::string_view Name) const
    {
        return std::all_of(this->ItemNames.begin(), this->ItemNames.end(), [&](std::string_view ItemName) { return ItemName.empty() || ItemName == Name; });
    }

    bool Container::ContainsOnly(const Interactable::Item& Item) const
    {
        return std::all_of(this->ItemIDs.begin(), this->ItemIDs.end(), [&](std::int32_t ItemID) { return ItemID == -1 || ItemID == Item.GetID(); });
    }

    bool Container::ContainsOnly(const std::vector<std::int32_t>& IDs) const
    {
        return false;
    }

    bool Container::ContainsOnly(const std::vector<std::string>& Names) const
    {
        return false;
    }

    bool Container::ContainsOnly(const std::vector<Interactable::Item>& InteractableItems) const
    {
        return false;
    }

    bool Container::IsEmpty() const
    {
        return this->CountEmpty() == this->Items.size();
    }

    const std::vector<std::int32_t>& Container::GetItemIDs() const { return this->ItemIDs; }
    const std::vector<std::string>& Container::GetItemNames() const { return this->ItemNames; }
    const std::vector<std::int32_t>& Container::GetItemAmounts() const { return this->ItemAmounts; }
    const std::vector<Item>& Container::GetItems() const { return this->Items; };

    Container::operator bool() const
    {
        switch (Source)
        {
            case INVENTORY:
            case EQUIPMENT:
            case BANK: return !this->Items.empty();
            default: break;
        }
        return false;
    }

    bool Container::operator==(const Container& I) const
    {
        return this->Source == I.Source
               && std::equal(this->ItemIDs.begin(),     this->ItemIDs.end(),     I.ItemIDs.begin(),     I.ItemIDs.end())
               && std::equal(this->ItemNames.begin(),   this->ItemNames.end(),   I.ItemNames.begin(),   I.ItemNames.end())
               && std::equal(this->ItemAmounts.begin(), this->ItemAmounts.end(), I.ItemAmounts.begin(), I.ItemAmounts.end());

    }

    bool Container::operator!=(const Container& I) const
    {
        return this->Source != I.Source
               || std::equal(this->ItemIDs.begin(),     this->ItemIDs.end(),     I.ItemIDs.begin(),     I.ItemIDs.end())
               || std::equal(this->ItemNames.begin(),   this->ItemNames.end(),   I.ItemNames.begin(),   I.ItemNames.end())
               || std::equal(this->ItemAmounts.begin(), this->ItemAmounts.end(), I.ItemAmounts.begin(), I.ItemAmounts.end());
    }
}