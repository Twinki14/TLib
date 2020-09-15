#ifndef TLIB_BUFFS_HPP
#define TLIB_BUFFS_HPP

#include <string>
#include <vector>
#include <map>
#include <Core/Types/Countdown.hpp>
#include <Game/Interactable/Item.hpp>

class Buffs
{
public:

    typedef enum Potion
    {
        EXTENDED_SUPER_ANTIFIRE,
        ANTI_VENOM_PLUS,
        DIVINE_MAGIC,
        DIVINE_RANGING,
        DIVINE_SUPER_ATTACK,
        DIVINE_SUPER_COMBAT,
        DIVINE_SUPER_DEFENCE,
        DIVINE_SUPER_STRENGTH
    } Potion;

    typedef struct PotionBuffDefinition
    {
        std::string Name;
        std::string DrinkMessage;
        std::uint64_t Duration;
        bool RemovedOnDeath;
        std::uint32_t StacksUpTo;
        std::uint32_t HealthDamage = 0;
        std::uint64_t LastDrinkTick = 0;
        bool CountdownStarted = false;
        Countdown C = Countdown();
    } PotionBuffDefinition;

    static void Scan();

    static bool Drink(const Potion& P, bool Quick = true, bool Spam = false);
    static bool CanDrink(const Potion& P, std::uint64_t Tolerence = 15 * 1000);
    static Interactable::Item GetPotion(const Potion& P);
    static std::int32_t CountPotions(const Potion& P, std::int32_t Stack = -1); // -1 = all dosages
    static std::uint64_t GetTimeLeft(const Potion& P);
    static double GetPercentageLeft(const Potion& P);
    static void OnDeath();

    static PotionBuffDefinition GetPotionBuffDefinition(const Potion& P);
private:
    static std::map<std::uint32_t, PotionBuffDefinition> Potions;
};

#endif //TLIB_BUFFS_HPP
