#include "../../Include/Utilities/Inventory.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include <Core/Internal.hpp>
#include <Game/Tools/Interact.hpp>
#include <Game/Tools/Profile.hpp>

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

/*    bool Move(const Interactable::Item& Item, std::int32_t ToSlot, bool AllowSwap)
    {
        if (!Item || ToSlot < 0 || ToSlot > 27) return false;
        if (Item.GetIndex() == ToSlot) return true;
        if (!AllowSwap && Inventory::GetItemIDs()[ToSlot] == -1) return false;

        auto SlotBoxes = Inventory::GetSlotBoxes();
        if (SlotBoxes.empty() || ToSlot >= SlotBoxes.size())
            return false;

        if (Item.Hover())
        {
            MouseDown(BUTTON_LEFT);
            Timer T;
            if (WaitFunc(650, 75, Internal::GetDraggingItem, true))
            {
                T.Suspend();
                std::int32_t ProfileWait = NormalRandom(Profile::GetMouseDownMean(), Profile::GetMouseDownMean() * Profile::GetMouseDownDeviation());
                std::int32_t TimePassed = ProfileWait - T.GetTimeElapsed();
                if (TimePassed > 0) Wait(TimePassed);

                auto SlotBox = Inventory::GetSlotBoxes()[ToSlot];
                if (Interact::MoveMouse(SlotBox.GetProfileHybridRandomPoint()))
                {
                    MouseUp(BUTTON_LEFT);
                    Wait(NormalRandom(Profile::GetMouseUpMean(), Profile::GetMouseUpMean() * Profile::GetMouseUpDeviation()));
                    return WaitFunc(250, 25, Internal::GetDraggingItem, false);
                }
            } else
            {
                MouseUp(BUTTON_LEFT);
                Wait(NormalRandom(Profile::GetMouseUpMean(), Profile::GetMouseUpMean() * Profile::GetMouseUpDeviation()));
                return false;
            }
        }
        return false;
    }*/

    bool Move(const Interactable::Item& Item, std::int32_t ToSlot, bool AllowSwap)
    {
        if (!Item || ToSlot < 0 || ToSlot > 27) return false;
        if (Item.GetIndex() == ToSlot) return true;
        if (!AllowSwap && Inventory::GetItemIDs()[ToSlot] == -1) return false;

        auto SlotBoxes = Inventory::GetSlotBoxes();
        if (SlotBoxes.empty() || ToSlot >= SlotBoxes.size())
            return false;

        Profile::Push(Profile::Var_RawInteractableMean, 125);
        if (Item.Hover())
        {
            Profile::Pop(1);
            MouseDown(BUTTON_LEFT);
            Wait(NormalRandom(225, 225 * 0.10));
            auto SlotBox = Inventory::GetSlotBoxes()[ToSlot];
            if (Interact::MoveMouse(SlotBox.GetProfileHybridRandomPoint()))
            {
                MouseUp(BUTTON_LEFT);
                Wait(NormalRandom(Profile::GetMouseUpMean(), Profile::GetMouseUpMean() * Profile::GetMouseUpDeviation()));
                return true;
            }
        }
        return false;
    }
}
