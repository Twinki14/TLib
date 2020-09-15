#ifndef TLIB_GEARSETS_HPP_INCLUDED
#define TLIB_GEARSETS_HPP_INCLUDED

#include <string>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include "OSRSBox/Schemas/Item.hpp"

/*
 * Aliases - IsCharged, IsEmpty, IsBroken, IsCharged, get broken/uncharged item util,
 * glory, ring of dueling
 * withdraw/equp needs to consider aliases - i think innamecache handles that
 */


namespace GearSets
{
    class Item
    {
    private:
        struct item_t
        {
            std::int32_t ID;
            std::string Name;
            OSRSBox::Items::Item Info;
            bool Broken;
        };
    public:
        explicit Item(std::string_view Name = "", std::int32_t ID = -1);

        std::string GetName() const { return Name; };
        std::int32_t GetID() const { return ID; };
        std::uint32_t GetRequiredSpecial() const { return RequiredSpecial; };

        bool IsChargeable() const { return Chargeable; };
        bool IsDegradable() const { return Degradable; };
        bool IsRepairable() const { return Repairable; };
        bool IsWeapon() const { return Info.equipableWeapon; };
        bool IsTwoHanded() const { return Info.equipment->slot == OSRSBox::Items::Slot::TwoHanded; };
        bool IsBroken() const; /// @return true if the item is unequipped, not found in inventory, and an item matching the id/name is found within the inventory
        bool IsEmpty() const; /// @return true if the item is unequipped, not found in inventory, and an item matching the id/name is found within the inventory

        bool IsSpecialWeapon() const { return RequiredSpecial > 0; };
        bool CanSpecial() const;

        std::uint32_t GetAttackSpeed() const { return Info.weapon->attackSpeed; };

        const std::vector<item_t>& GetChargedAliases() const { return ChargedAliases; };
        const std::vector<item_t>& GetUnchargedAliases() const { return UnchargedAliases; };
        const std::vector<item_t>& GetDegradedAliases() const { return DegradedAliases; };
        const std::vector<item_t>& GetBrokenAliases() const { return BrokenAliases; };
        const OSRSBox::Items::Item& GetInfo() const { return Info; };

        operator bool() const { return !Name.empty() && Name != "NULL" && ID >= 0; };

        ~Item() = default;
    private:
        std::string Name;
        std::int32_t ID;
        std::uint32_t RequiredSpecial = 0;

        bool Chargeable = false;
        bool Degradable = false;
        bool Repairable = false;

        std::vector<item_t> ChargedAliases; // bool IsCharged - TODO
        std::vector<item_t> UnchargedAliases;

        std::vector<item_t> DegradedAliases;
        std::vector<item_t> BrokenAliases;

        OSRSBox::Items::Item Info;

/*    private:
        double (*GetChargesFunc)(GearSet::Item *I) = nullptr;
        std::pair<double, double> (*GetCharges_PairFunc)(GearSet::Item *I) = nullptr;
        double (*GetDegradeProgressFunc)(GearSet::Item *I) = nullptr;
        bool (*ChargeFunc)(GearSet::Item *I) = nullptr;*/
    };

    class Set
    {
    public:

        bool Equip(const std::function<bool()>& StopEarly = nullptr) const; // AllowAny = equip even if it isn't eq
        bool Withdraw() const;

        bool CanSpecial() const; /// @return true if the weapon of the set has a special attack, and has enough energy to cast it
        bool UseSpecial() const;

        bool IsBroken() const; /// @return true if any items are degrable and are broken
        bool IsEmpty() const; /// @return true if any items are degrable and are broken

        bool Holding() const; /// @return true if all items are either equipped or in inventory
        bool Equipped() const; /// @return true if all items are equipped
        bool InInventory() const; /// @return true if all items are in inventory
        bool InBank() const; /// @return true if all items are in bank

        std::uint32_t CountUnequipped() const; /// @return how many gear items are not equipped
        std::uint32_t CountMissing() const; // @return how many gear items are completely missing from the currently accessible item containers (bank if open), inventory, and equipment

        std::map<std::uint32_t, GearSets::Item> Items; // map will be ordered by Equipment:: enum
        OSRSBox::Items::Equipment TotalEquipmentInfo;
    };

    inline std::map<std::string, GearSets::Set> Sets;
    std::uint32_t SetFromEquipped(const std::string& Key); // returns size of created GearSet
}

#endif // TLIB_GEARSETS_HPP_INCLUDED