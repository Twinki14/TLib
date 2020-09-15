#include "../../Include/Utilities/Food.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include <Game/Interfaces/GameTabs/Stats.hpp>
#include <Game/Interfaces/GameTabs/Inventory.hpp>
#include <Core/Time.hpp>
#include <cmath>

std::map<Food::FOOD, Food::FOOD_DEF> Food::FoodDefs =
        {
                { Food::LOBSTER, { "Lobster", 379, 12, true, false } },
                { Food::SWORDFISH, { "Swordfish", 373, 14, true, false } },
                { Food::MONKFISH, { "Monkfish", 7946, 16, true, false } },
                { Food::COOKED_KARAMBWAN, { "Cooked karambwan", 3144, 18, false, false } },
                { Food::SHARK, { "Shark", 385, 20, true, false } },
                { Food::MANTA_RAY, { "Manta ray", 391, 22, true, false } },
                { Food::ANGLERFISH, { "Anglerfish", 13441, 22, true, true } }
        };

std::string Food::GetName(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
        return Food::FoodDefs[Food].Name;
    return "";
}

std::int32_t Food::GetItemID(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
        return Food::FoodDefs[Food].ItemID;
    return -1;
}

std::uint32_t Food::GetHealthRestore(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
    {
        if (Food == Food::ANGLERFISH)
        {
            const auto HPLevel = Stats::GetRealLevel(Stats::HITPOINTS);
            //if (HPLevel < 10) return 0;
            if (HPLevel <= 19) return 3;
            if (HPLevel <= 24) return 4;
            if (HPLevel <= 29) return 6;
            if (HPLevel <= 39) return 7;
            if (HPLevel <= 49) return 8;
            if (HPLevel <= 59) return 11;
            if (HPLevel <= 69) return 12;
            if (HPLevel <= 79) return 15;
            if (HPLevel <= 89) return 16;
            if (HPLevel <= 92) return 17;
            if (HPLevel <= 99) return 22;
        }
        return Food::FoodDefs[Food].BaseHeal;
    }
    return 0;
}

bool Food::ConsumesTick(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
        return Food::FoodDefs[Food].TakesTick;
    return false;
}

bool Food::CanOverheal(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
        return Food::FoodDefs[Food].Overheals;
    return false;
}

Interactable::Item Food::GetItem(const FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
        return Inventory::GetItem(Food::GetItemID(Food));
    return Interactable::Item();
}

bool Food::Eat(const Food::FOOD& Food, bool Quick, bool Spam)
{
    if (Food::FoodDefs.count(Food))
    {
        const auto Item = Food::GetItem(Food);
        if (!Item) return false;

        if (Spam)
        {
            auto Eat = [&]() -> bool
            {
                if (!Item.Exists()) return true;
                if (Quick) Profile::Push(Profile::Var_RawInteractableMean, 50);
                if (Inventory::Open(Profile::RollUseGametabHotKey()))
                    Item.Interact("Eat");
                if (Quick) Profile::Pop();
                return !Item.Exists();
            };
            return WaitFunc(2250, 50, Eat, true);
        } else
        {
            if (Quick) Profile::Push(Profile::Var_RawInteractableMean, 50);
            if (Inventory::Open(Profile::RollUseGametabHotKey()))
                Item.Interact("Eat");
            if (Quick) Profile::Pop();
            auto Ate = [&]() -> bool { return !Item.Exists(); };
            return WaitFunc(650, 25, Ate, true);
        }
    }
    return false;
}

bool Food::QuickEat(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
    {
        const auto Item = Food::GetItem(Food);
        if (!Item) return false;

        Profile::Push(Profile::Var_RawInteractableMean, 0);
        Profile::Push(Profile::Var_RawMoveMean, 50);
        bool Clicked = Inventory::Open(Profile::RollUseGametabHotKey()) && Item.Interact("Eat");
        Profile::Pop(2);
        return Clicked;
    }
    return false;
}

bool Food::ComboEat(const Food::FOOD& Food, const Food::FOOD& ComboFood, bool AllowWaste)
{
    if (Food::FoodDefs.count(Food))
    {
        std::int32_t FoodHeals = Food::GetHealthRestore(Food);
        std::int32_t ComboFoodHeals = Food::GetHealthRestore(ComboFood);
        std::int32_t Difference = Stats::GetRealLevel(Stats::HITPOINTS) - Stats::GetCurrentLevel(Stats::HITPOINTS);

        bool UseCombo = Food::FoodDefs.count(ComboFood) > 0 && !Food::ConsumesTick(ComboFood);
        bool HasFood = Food::Count(Food) > 0;
        bool HasComboFood = UseCombo && Food::Count(ComboFood) > 0;

        if (!HasFood && !HasComboFood) return false;

        if (HasComboFood)
        {
            if (HasFood)
                UseCombo = (ComboFoodHeals + FoodHeals) <= Difference || AllowWaste;
            else
                UseCombo = ComboFoodHeals <= Difference || AllowWaste;
        }

        if (UseCombo)
            Food::QuickEat(ComboFood);
        else if (!HasFood)
            return true;

        if (!AllowWaste)
        {
            if (FoodHeals <= Difference)
                return Food::Eat(Food, true, true);
            else
                return true;
        } else
            return Food::Eat(Food, true, true);
    }
    return false;
}

bool Food::EatUntil(const FOOD& Food, const FOOD& ComboFood, double NeededHealth, bool AllowWaste)
{
    std::int32_t RawHealth = std::ceil(Stats::GetRealLevel(Stats::HITPOINTS) * NeededHealth);
    return Food::EatUntil(Food, ComboFood, RawHealth, AllowWaste);
}

bool Food::EatUntil(const FOOD& Food, const FOOD& ComboFood, std::int32_t NeededHealth, bool AllowWaste)
{
    if (Food::FoodDefs.count(Food))
    {
        std::int32_t FoodHeals = Food::GetHealthRestore(Food);
        std::int32_t ComboFoodHeals = Food::GetHealthRestore(ComboFood);
        std::int32_t CurrentHealth = Stats::GetCurrentLevel(Stats::HITPOINTS);
        std::int32_t Difference = NeededHealth - CurrentHealth;

        while (Difference > 0)
        {
            //if (Terminate) return false;

            bool UseCombo = Food::FoodDefs.count(ComboFood) > 0 && !Food::ConsumesTick(ComboFood);
            bool HasFood = Food::Count(Food) > 0;
            bool HasComboFood = UseCombo && Food::Count(ComboFood) > 0;

            if (!HasFood && !HasComboFood) return false;

            if (HasComboFood)
            {
                if (HasFood)
                    UseCombo = (ComboFoodHeals + FoodHeals) <= Difference;
                else
                    UseCombo = ComboFoodHeals <= Difference || AllowWaste;
            }

            if (UseCombo)
                Food::QuickEat(ComboFood);
            else if (!HasFood)
                return true;

            CurrentHealth = Stats::GetCurrentLevel(Stats::HITPOINTS);
            Difference = NeededHealth - CurrentHealth;

            if (!AllowWaste)
            {
                if (FoodHeals <= Difference)
                    Food::Eat(Food, true, true);
                else
                    return true;
            } else
                Food::Eat(Food, true, true);

            CurrentHealth = Stats::GetCurrentLevel(Stats::HITPOINTS);
            Difference = NeededHealth - CurrentHealth;
            if (Difference <= 0) return true;
        }
        return Difference <= 0;
    }
    return false;
}

std::uint32_t Food::Count(const Food::FOOD& Food)
{
    if (Food::FoodDefs.count(Food))
    {
        std::int32_t Count = Inventory::Count(Food::FoodDefs[Food].ItemID);
        if (Count < 0) Count = 0;
        return (std::uint32_t) Count;
    }
    return 0;
}
