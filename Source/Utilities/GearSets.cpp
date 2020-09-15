#include "../../Include/Utilities/GearSets.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include "../../Include/TScript.hpp"
#include "../../Include/Utilities/Containers.hpp"
#include <Game/Interfaces/GameTabs/Combat.hpp>
#include <Game/Interfaces/GameTabs/Equipment.hpp>
#include <Game/Interfaces/GameTabs/Inventory.hpp>
#include <Game/Tools/Profile.hpp>
#include <Game/Tools/Widgets.hpp>
#include <Core/Types/Countdown.hpp>
#include <Game/Interfaces/Bank.hpp>
#include <Core/Globals.hpp>
#include <OSRSBox/Items.hpp>

// TODO
//
//  Some weapons will have a chat message displayed if the item is out of charges, upon attacking
//      Could implement an OnAttack() function that's used outside of the class, to check the most recent messages up to 5s ago
//      if any of the messages match a message indicating the weapon or w/e is out of charges, then mark the set as UNCHARGED or something
//

GearSets::Item::Item(std::string_view Name, std::int32_t ID) : Name(Name), ID(ID)
{
    if (this->Name.empty() && this->ID < 0)
    {
        this->Name = "NULL";
        this->ID = -1;
        return;
    }

    if (this->ID < 0 && !this->Name.empty())
    {
        this->ID = OSRSBox::Items::Find(this->Name, false, false, true);
        if (this->ID < 0)
        {
            this->Name = "NULL";
            this->ID = -1;
            return;
        }
    }

    if (this->Name.empty() && this->ID >= 0)
    {
        this->Name = OSRSBox::Items::Find(this->ID, true);
        if (this->Name.empty())
        {
            this->Name = "NULL";
            this->ID = -1;
            return;
        }
    }

    bool RingOfSuffering = this->Name.find("Ring of suffering") != std::string::npos;
    bool Blowpipe = this->Name.find("Toxic blowpipe") != std::string::npos;
    bool SwampTrident = (this->Name.find("Uncharged toxic trident") != std::string::npos) || (this->Name.find("Trident of the swamp") != std::string::npos);
    bool SeaTrident = (this->Name.find("Uncharged trident") != std::string::npos) || (this->Name.find("Trident of the seas") != std::string::npos);
    bool ChargeableItem = Blowpipe || SwampTrident || SeaTrident || RingOfSuffering;

    if (ChargeableItem)
    {
        if (RingOfSuffering)
        {
            bool Imbued = this->Name.find("i)") != std::string::npos;
            this->Name = Imbued ? "Ring of suffering (ri)" : "Ring of suffering (r)";

            this->Chargeable = true;
            std::string UnchargedName = Imbued ? "Ring of suffering (i)" : "Ring of suffering";
            std::int32_t UnchargedID = OSRSBox::Items::Find(UnchargedName, false, false, true);
            this->UnchargedAliases.emplace_back( item_t{ UnchargedID, UnchargedName, OSRSBox::Items::Get(UnchargedID), false });
            //this->GetChargesFunc = &CHARGEABLE_GET_CHARGES_ROS;
        }

        if (Blowpipe)
        {
            this->Name = "Toxic blowpipe";
            this->Chargeable = true;

            std::string UnchargedName = "Toxic blowpipe (empty)";
            std::int32_t UnchargedID = OSRSBox::Items::Find(UnchargedName, false, false, true);
            this->UnchargedAliases.emplace_back( item_t{ UnchargedID, UnchargedName, OSRSBox::Items::Get(UnchargedID), false });
            //this->GetCharges_PairFunc = &CHARGEABLE_GET_CHARGES_BLOWPIPE;
            this->RequiredSpecial = 50;
        }

        if (SwampTrident)
        {
            bool Enhanced = this->Name.find("(e)") != std::string::npos;
            this->Name = Enhanced ? "Trident of the swamp (e)" : "Trident of the swamp";

            this->Chargeable = true;
            std::string UnchargedName = Enhanced ? "Uncharged toxic trident (e)" : "Uncharged toxic trident";
            std::int32_t UnchargedID = OSRSBox::Items::Find(UnchargedName, false, false, true);
            this->UnchargedAliases.emplace_back( item_t{ UnchargedID, UnchargedName, OSRSBox::Items::Get(UnchargedID), false });
            //this->GetChargesFunc = &CHARGEABLE_GET_CHARGES_TRIDENT;
        }

        if (SeaTrident)
        {
            bool Enhanced = this->Name.find("(e)") != std::string::npos;
            this->Name = Enhanced ? "Trident of the seas (e)" : "Trident of the seas";

            this->Chargeable = true;
            std::string UnchargedName = Enhanced ? "Uncharged trident (e)" : "Uncharged trident";
            std::int32_t UnchargedID = OSRSBox::Items::Find(UnchargedName, false, false, true);
            this->UnchargedAliases.emplace_back( item_t{ UnchargedID, UnchargedName, OSRSBox::Items::Get(UnchargedID), false });
            //this->GetChargesFunc = &CHARGEABLE_GET_CHARGES_TRIDENT;
        }
    }

    bool BGS = this->Name.find("Bandos godsword") != std::string::npos;
    bool DWH = this->Name.find("Dragon warhammer") != std::string::npos;
    bool DClaws = this->Name.find("Dragon claws") != std::string::npos;
    if (BGS) this->RequiredSpecial = 50;
    if (DWH) this->RequiredSpecial = 50;
    if (DClaws) this->RequiredSpecial = 50;

    bool MSB = this->Name.find("Magic shortbow") != std::string::npos;
    bool MSBI = this->Name.find("Magic shortbow (i)") != std::string::npos;
    if (MSB) this->RequiredSpecial = 55;
    if (MSBI) this->RequiredSpecial = 50;

    bool Dharok = this->Name.find("Dharok's") != std::string::npos;
    bool Torag = this->Name.find("Torag's") != std::string::npos;
    bool Guthan = this->Name.find("Guthan's") != std::string::npos;
    bool Verac = this->Name.find("Verac's") != std::string::npos;
    bool Karil = this->Name.find("Karil's") != std::string::npos;
    bool Ahrim = this->Name.find("Ahrim's") != std::string::npos;
    bool BarrowsItem = Dharok || Torag || Guthan || Verac || Karil || Ahrim;
    if (BarrowsItem)
    {
        this->Name.erase(std::remove_if(this->Name.begin(), this->Name.end(), &isdigit), this->Name.end());
        while (this->Name.back() == ' ')
            this->Name.pop_back();

        this->Degradable = true;
        std::vector<std::string> BarrowsNames;
        std::vector<std::int32_t> BarrowsIDs;
        for (std::uint32_t I = 0; I < 5; I++)
        {
            std::string N = this->Name + " " + std::to_string(I * 25);
            BarrowsIDs.emplace_back(OSRSBox::Items::Find(N, false, false, true));
            BarrowsNames.emplace_back(std::move(N));
        }

        for (std::uint32_t I = 0; I < BarrowsIDs.size(); I++)
            this->DegradedAliases.emplace_back(item_t { BarrowsIDs[I], std::move(BarrowsNames[I]), OSRSBox::Items::Get(BarrowsIDs[I]), I == 0 });
        //this->GetDegradeProgressFunc = &DEGRADEABLE_GET_PROG_BARROWS;
    }

    this->ID = OSRSBox::Items::Find(this->Name, false, false, true);
    this->Info = OSRSBox::Items::Get(this->ID);
}

bool GearSets::Item::IsBroken() const
{
    return false;
}

bool GearSets::Item::IsEmpty() const
{
    return false;
}

bool GearSets::Item::CanSpecial() const
{
    if (this->RequiredSpecial > 0)
        return Combat::GetSpecialAttack() >= this->RequiredSpecial;
    return false;
}

namespace GearSets
{
    bool InContainer(const Containers::Container& C, const GearSets::Item& Item, Containers::Item *Found = nullptr)
    {
        if (!C || C.IsEmpty()) return false;

        std::int32_t FoundIndex = C.GetIndexOf(Item.GetID());
        if (FoundIndex >= 0)
        {
            if (Found) *Found = C.GetItems()[FoundIndex];
            return true;
        }

        if (Item.IsChargeable())
        {
            for (auto& Charged : Item.GetChargedAliases())
            {
                FoundIndex = C.GetIndexOf(Charged.ID);
                if (FoundIndex >= 0)
                {
                    if (Found) *Found = C.GetItems()[FoundIndex];
                    return true;
                }
            }
        }

        if (Item.IsDegradable())
        {
            for (auto& Degraded : Item.GetDegradedAliases())
            {
                FoundIndex = C.GetIndexOf(Degraded.ID);
                if (FoundIndex >= 0)
                {
                    if (Found) *Found = C.GetItems()[FoundIndex];
                    return true;
                }
            }
        }

        return false;
    }

    bool InContainer_Broken(const Containers::Container& C, const GearSets::Item& Item, Containers::Item *Found = nullptr)
    {
        if (!C || C.IsEmpty()) return false;

        std::int32_t FoundIndex = 0;

        if (Item.IsChargeable())
        {
            for (auto& Uncharged : Item.GetUnchargedAliases())
            {
                FoundIndex = C.GetIndexOf(Uncharged.ID);
                if (FoundIndex >= 0)
                {
                    if (Found) *Found = C.GetItems()[FoundIndex];
                    return true;
                }
            }
        }

        if (Item.IsDegradable())
        {
            for (auto& Broken : Item.GetBrokenAliases())
            {
                FoundIndex = C.GetIndexOf(Broken.ID);
                if (FoundIndex >= 0)
                {
                    if (Found) *Found = C.GetItems()[FoundIndex];
                    return true;
                }
            }
        }

        return false;
    }
}

bool GearSets::Set::Equip(const std::function<bool()>& StopEarly) const
{
    auto InventoryContainer = Containers::Container(Containers::INVENTORY);
    auto EquipmentContainer = Containers::Container(Containers::EQUIPMENT);

    std::vector<std::int32_t> UnequippedItems;
    for (const auto& [EquipmentSlot, Item] : this->Items)
    {
        if (!Item) continue;

        if (!InContainer(EquipmentContainer, Item))
        {
            Containers::Item Found;
            if (InContainer(InventoryContainer, Item, &Found))
                UnequippedItems.emplace_back(Found.GetID());
        }
    }

    if (UnequippedItems.empty()) return true;

    if (Bank::IsOpen() && !Bank::Close(Profile::RollUseEscHotkey()))
        return false;

    auto InvItems = Inventory::GetItems(UnequippedItems);
    if (InvItems.empty())
        return false;

    const auto InteractableMean = Profile::GetRawInteractableMean();
    const auto InteractableDev = Profile::GetRawInteractableDeviation();

    Profile::SetRawInteractableMean(50);
    Profile::SetRawInteractableDeviation(0.10);

    if (!Inventory::Open(Profile::RollUseGametabHotKey()))
        return false;

    static const std::vector<std::string> Options = { "Wield", "Equip", "Wear" };
    for (const auto& I : InvItems)
    {
        if (StopEarly && StopEarly()) return false;
        I.Interact(Options);
    }

    Profile::SetRawInteractableMean(InteractableMean);
    Profile::SetRawInteractableDeviation(InteractableDev);

    Countdown C(1000);
    while (C)
    {
        if (this->Equipped()) return true;
        if (StopEarly && StopEarly()) return false;
        Wait(50);
    }
    return false;
}

bool GearSets::Set::Withdraw() const
{
    if (!Bank::IsOpen())
        return false;

    auto InventoryContainer = Containers::Container(Containers::INVENTORY);
    auto EquipmentContainer = Containers::Container(Containers::EQUIPMENT);
    auto BankContainer = Containers::Container(Containers::BANK);

    std::vector<std::int32_t> UnequippedItems;
    for (const auto& [EquipmentSlot, Item] : this->Items)
    {
        if (!Item) continue;

        if (!InContainer(EquipmentContainer, Item) && !InContainer(InventoryContainer, Item))
        {
            Containers::Item Found;
            if (InContainer(BankContainer, Item, &Found))
                UnequippedItems.emplace_back(Found.GetID());
            else
                return false;
        }
    }

    if (UnequippedItems.empty()) return true;

    auto BankItems = Bank::GetItems(UnequippedItems);
    if (BankItems.empty())
        return false;

    const auto InteractableMean = Profile::GetRawInteractableMean();

    Profile::SetRawInteractableMean(200);
    for (const auto& I : BankItems)
    {
        bool Stackable = false;
        const auto Name = I.GetName();

        if (Name.find(" arrow") != std::string::npos || Name.find(" arrows") != std::string::npos)
            Stackable = true;
        if (Name.find(" bolt") != std::string::npos || Name.find(" bolts") != std::string::npos)
            Stackable = true;

        if (Stackable)
            Bank::Withdraw(I, Bank::ALL_BUT_ONE, true);
        else
            Bank::Withdraw(I, 1, true);
    }

    Profile::SetRawInteractableMean(InteractableMean);

    Countdown C(5000);
    while (C)
    {
        if (this->Holding())
            return true;
        Wait(50);
    }

    return false;
}

bool GearSets::Set::CanSpecial() const
{
    return this->Items.at(Equipment::WEAPON).CanSpecial();
}

bool GearSets::Set::UseSpecial() const
{
    if (Bank::IsOpen() && !Bank::Close(Profile::RollUseEscHotkey()))
        return false;

    if (!this->Equipped())
        return false;

    if (this->CanSpecial())
    {
        if (Combat::IsSpecialAttacking())
            return true;

        const auto StartingSpecPoints = Combat::GetSpecialAttack();
        const auto InteractableMean = Profile::GetRawInteractableMean();
        const auto MoveMean = Profile::GetRawMoveMean();

        bool Result = false;
        Profile::SetRawInteractableMean(50);
        Profile::SetRawMoveMean(50);

        if (!Combat::IsOpen())
        {
            const auto MinimapOrb = Widgets::Get(160, 30);
            if (MinimapOrb.IsVisible() && MinimapOrb.Interact("Use Special Attack"))
                Result = WaitFunc(650, 15, Combat::IsSpecialAttacking, true) || StartingSpecPoints > Combat::GetSpecialAttack();
        }

        if (!Result)
        {
            if (!Combat::Open())
                return false;

            const auto W = Widgets::Get(Globals::GameTabs::COMBAT_SPECIAL.Parent, Globals::GameTabs::COMBAT_SPECIAL.Child);
            Result = (W.Interact("Use Special Attack") && WaitFunc(650, 15, Combat::IsSpecialAttacking, true)) || StartingSpecPoints > Combat::GetSpecialAttack();
        }

        Profile::SetRawInteractableMean(InteractableMean);
        Profile::SetRawMoveMean(MoveMean);

        return Result;
    }
    return false;
}

bool GearSets::Set::Holding() const
{
    auto InventoryContainer = Containers::Container(Containers::INVENTORY);
    auto EquipmentContainer = Containers::Container(Containers::EQUIPMENT);

    return std::all_of(this->Items.begin(), this->Items.end(), [&](const std::pair<std::uint32_t, Item>& I)
    { return InContainer(InventoryContainer, I.second) || InContainer(EquipmentContainer, I.second); });
}

bool GearSets::Set::Equipped() const
{
    auto EquipmentContainer = Containers::Container(Containers::EQUIPMENT);
    return std::all_of(this->Items.begin(), this->Items.end(), [&](const std::pair<std::uint32_t, Item>& I) { return InContainer(EquipmentContainer, I.second); });
}

bool GearSets::Set::InInventory() const
{
    auto InventoryContainer = Containers::Container(Containers::INVENTORY);
    return std::all_of(this->Items.begin(), this->Items.end(), [&](const std::pair<std::uint32_t, Item>& I) { return InContainer(InventoryContainer, I.second); });
}

bool GearSets::Set::InBank() const
{
    if (!Bank::IsOpen())
        return false;

    auto BankContainer = Containers::Container(Containers::BANK);
    return std::all_of(this->Items.begin(), this->Items.end(), [&](const std::pair<std::uint32_t, Item>& I)
    {
        Containers::Item Found;
        return InContainer(BankContainer, I.second, &Found) && Found.GetAmount() > 0;
    });
}

std::uint32_t GearSets::Set::CountUnequipped() const
{
    auto EquipmentContainer = Containers::Container(Containers::EQUIPMENT);
    return std::count_if(this->Items.begin(), this->Items.end(), [&](const std::pair<std::uint32_t, Item>& I) { return InContainer(EquipmentContainer, I.second); });
}

std::uint32_t GearSets::Set::CountMissing() const
{
    auto InventoryContainer = Containers::Container(Containers::INVENTORY);
    auto EquipmentContainer = Containers::Container(Containers::EQUIPMENT);
    auto BankContainer = Bank::IsOpen() ? Containers::Container(Containers::BANK) : Containers::Container();
    return std::count_if(this->Items.begin(), this->Items.end(), [&](const std::pair<std::uint32_t, Item>& I)
    {
        Containers::Item Found;
        return !InContainer(InventoryContainer, I.second) && !InContainer(EquipmentContainer, I.second) && (!InContainer(BankContainer, I.second, &Found) || Found.GetAmount() <= 0);
    });
}

std::uint32_t GearSets::SetFromEquipped(const std::string& Key)
{
    Set S;

    auto EquippedItemNames = Equipment::GetItemNames();
    auto EquippedItemIDs = Equipment::GetItemIDs();
    if (EquippedItemNames.size() != EquippedItemIDs.size()) return 0;

    for (std::uint32_t I = Equipment::HEAD; I <= Equipment::AMMO; I++)
        S.Items[I] = Item(EquippedItemNames[I], EquippedItemIDs[I]);

    for (const auto& [Slot, I] : S.Items)
    {
        if (I.GetInfo().equipment)
        {
            S.TotalEquipmentInfo.attackStab        +=  I.GetInfo().equipment->attackStab;
            S.TotalEquipmentInfo.attackSlash       +=  I.GetInfo().equipment->attackSlash;
            S.TotalEquipmentInfo.attackCrush       +=  I.GetInfo().equipment->attackCrush;
            S.TotalEquipmentInfo.attackMagic       +=  I.GetInfo().equipment->attackMagic;
            S.TotalEquipmentInfo.attackRanged      +=  I.GetInfo().equipment->attackRanged;
            S.TotalEquipmentInfo.defenceStab       +=  I.GetInfo().equipment->defenceStab;
            S.TotalEquipmentInfo.defenceSlash      +=  I.GetInfo().equipment->defenceSlash;
            S.TotalEquipmentInfo.defenceCrush      +=  I.GetInfo().equipment->defenceCrush;
            S.TotalEquipmentInfo.defenceMagic      +=  I.GetInfo().equipment->defenceMagic;
            S.TotalEquipmentInfo.defenceRanged     +=  I.GetInfo().equipment->defenceRanged;
            S.TotalEquipmentInfo.meleeStrength     +=  I.GetInfo().equipment->meleeStrength;
            S.TotalEquipmentInfo.rangedStrength    +=  I.GetInfo().equipment->rangedStrength;
            S.TotalEquipmentInfo.magicDamage       +=  I.GetInfo().equipment->magicDamage;
            S.TotalEquipmentInfo.prayer            +=  I.GetInfo().equipment->prayer;
        }
    }
    
    std::uint32_t Count = 0;
    for (const auto& Item : S.Items)
        if (Item.second)
            Count++;

    GearSets::Sets.insert_or_assign(Key, std::move(S));
    return Count;
}
