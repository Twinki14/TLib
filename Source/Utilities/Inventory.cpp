#include "../../Include/Utilities/Inventory.hpp"
#include <Core/Internal.hpp>

namespace Inventory
{
    std::vector<CONTAINER_ITEM> GetContainerItems(bool IncludeEmpty)
    {
        std::vector<CONTAINER_ITEM> Result;

        const auto ItemIDs = Inventory::GetItemIDs();
        const auto ItemAmounts = Inventory::GetItemAmounts();

        if (ItemIDs.size() != ItemAmounts.size())
            return Result;

        for (std::uint32_t I = 0; I < ItemIDs.size(); I++)
        {
            if (ItemIDs[I] > -1)
            {
                std::string Name;
                const auto Info = Internal::GetItemInfo(ItemIDs[I]);
                if (Info)
                    Name = Info.GetName();
                else
                    Name = Internal::GetItemName(ItemIDs[I]);

                Result.emplace_back( CONTAINER_ITEM { ItemIDs[I], std::move(Name), ItemAmounts[I], I } );
            } else if (IncludeEmpty)
            {
                Result.emplace_back( CONTAINER_ITEM { ItemIDs[I], "", ItemAmounts[I], I } );
            }
        }

        return Result;
    }

    std::int32_t Count(std::int32_t ID, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::int32_t Count = 0;
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
            if (ItemID == ID)
                Count += ItemAmount;
        return Count;
    }

    std::int32_t Count(const std::string& Name, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::int32_t Count = 0;
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
            if (ItemName == Name)
                Count += ItemAmount;
        return Count;
    }

    std::int32_t Count(const Interactable::Item& Item, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        if (Item.GetSource() != Interactable::Item::BANK)
            return 0;

        std::int32_t Count = 0;
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
            if (ItemID == Item.GetID())
                Count += ItemAmount;

        return Count;
    }

    std::vector<std::int32_t> CountIndividual(const std::vector<std::int32_t>& IDs, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::vector<std::int32_t> Result = std::vector<std::int32_t>(IDs.size(), 0);
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
        {
            if (ItemID < 0) continue;
            for (std::uint32_t I = 0; I < IDs.size(); I++)
                if (ItemID == IDs[I])
                    Result[I] += ItemAmount;
        }
        return Result;
    }

    std::vector<std::int32_t> CountIndividual(const std::vector<std::string>& Names, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::vector<std::int32_t> Result = std::vector<std::int32_t>(Names.size(), 0);
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
        {
            if (ItemID < 0) continue;
            for (std::uint32_t I = 0; I < Names.size(); I++)
                if (ItemName == Names[I])
                    Result[I] += ItemAmount;
        }
        return Result;
    }

    std::vector<std::int32_t> CountIndividual(const std::vector<Interactable::Item>& Items, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::vector<std::int32_t> Result = std::vector<std::int32_t>(Items.size(), 0);
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
        {
            if (ItemID < 0) continue;
            for (std::uint32_t I = 0; I < Items.size(); I++)
            {
                if (Items[I].GetSource() != Interactable::Item::BANK)
                    continue;
                if (ItemID == Items[I].GetID())
                    Result[I] += ItemAmount;
            }
        }
        return Result;
    }
}
