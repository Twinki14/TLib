#ifndef TLIB_BOOSTS_HPP
#define TLIB_BOOSTS_HPP

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <Game/Interactable/Item.hpp>
#include <Game/Interfaces/GameTabs/Stats.hpp>

class Boosts
{
public:

    typedef enum POTION
    {
        SUPER_ATTACK,
        SUPER_STRENGTH,
        SUPER_DEFENCE,
        RANGING,
        MAGIC,
        BASTION,
        BATLLEMAGE,
        SUPER_COMBAT
    } POTION;

    static bool Drink(const POTION& P, bool Quick = true, bool Spam = false);
    static bool Has(const POTION& P, std::uint32_t Tolerence = 3);
    static std::int32_t CountPotions(const POTION& P, std::int32_t Stack = -1); // -1 = all dosages
    static Interactable::Item GetPotion(const POTION& P);
    static std::vector<std::tuple<std::string, std::uint32_t, std::uint32_t>> GetStatus(const POTION& P); // Name, Current, Real,

private:

    typedef struct BoostPotion
    {
        std::string Name;
        std::vector<Stats::SKILLS> Skills;
        std::uint32_t MinimumBoost;
        std::uint32_t StacksTo;
    } BoostPotion;

    static std::map<std::uint32_t, Boosts::BoostPotion> Potions;
};

#endif //TLIB_BOOSTS_HPP
