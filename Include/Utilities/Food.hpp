#ifndef TLIB_FOOD_HPP
#define TLIB_FOOD_HPP

#include <Game/Interactable/Item.hpp>
#include <map>
#include <vector>

class Food
{
public:

    enum FOOD
    {
        NILL = -1,
        LOBSTER,
        SWORDFISH,
        MONKFISH,
        COOKED_KARAMBWAN,
        SHARK,
        MANTA_RAY,
        ANGLERFISH
    };

    static std::string GetName(const FOOD& Food);
    static std::int32_t GetItemID(const FOOD& Food);
    static std::uint32_t GetHealthRestore(const FOOD& Food);
    static bool ConsumesTick(const FOOD& Food);
    static bool CanOverheal(const FOOD& Food);
    static Interactable::Item GetItem(const FOOD& Food);
    static bool Eat(const FOOD& Food, bool Quick = true, bool Spam = true);
    static bool QuickEat(const FOOD& Food); // Will click the food once, returns as soon as it clicks
    static bool ComboEat(const FOOD& Food, const FOOD& ComboFood, bool AllowWaste = false);
    static bool EatUntil(const FOOD& Food, const FOOD& ComboFood, double NeededHealth, bool AllowWaste = false);
    static bool EatUntil(const FOOD& Food, const FOOD& ComboFood, std::int32_t NeededHealth, bool AllowWaste = false);
    static std::uint32_t Count(const FOOD& Food);

private:
    typedef struct FOOD_DEF
    {
        std::string Name;
        std::uint32_t ItemID;
        std::uint32_t BaseHeal;
        bool TakesTick;
        bool Overheals;
    } FOOD_DEF;
public:
    static std::map<Food::FOOD, Food::FOOD_DEF> FoodDefs;
};

#endif //TLIB_FOOD_HPP
