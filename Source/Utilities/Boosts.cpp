#include "../../Include/Utilities/Boosts.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include <Core/Time.hpp>
#include <Game/Interfaces/GameTabs/Inventory.hpp>

std::map<std::uint32_t, Boosts::BoostPotion> Boosts::Potions =
{
        { SUPER_ATTACK,     { "Super attack(",              { Stats::ATTACK },      5, 4 } },
        { SUPER_STRENGTH,   { "Super strength(",            { Stats::STRENGTH },    5, 4 } },
        { SUPER_DEFENCE,    { "Super defence(",             { Stats::DEFENCE },     5, 4 } },
        { RANGING,          { "Ranging potion(",            { Stats::RANGE },       4, 4 } },
        { MAGIC,            { "Magic potion(",              { Stats::MAGIC },       4, 4 } },
        { BASTION,          { "Bastion potion(",            { Stats::RANGE, Stats::DEFENCE }, 4, 4 } },
        { BATLLEMAGE,       { "Battlemage potion(",         { Stats::MAGIC, Stats::DEFENCE }, 4, 4 } },
        { SUPER_COMBAT,     { "Super combat potion(",       { Stats::ATTACK, Stats::STRENGTH, Stats::DEFENCE }, 5, 4 } },
};

static const std::vector<std::string> SkillNames =
{
        "Attack", "Defence", "Strength", "Hitpoints", "Range", "Prayer", "Magic", "Cooking", "Woodcutting",
        "Fletching", "Fishing", "Firemaking", "Crafting", "Smithing", "Mining", "Herblore", "Agility",
        "Thieving", "Slayer", "Farming", "Runecraft", "Hunter", "Construction", "All"
};

bool Boosts::Drink(const Boosts::POTION& P, bool Quick, bool Spam)
{
    const auto Pot = Boosts::GetPotion(P);
    if (!Pot) return false;

    if (Spam)
    {
        auto DoDrink = [&]() -> bool
        {
            if (Boosts::Has(P, 0)) return true;
            if (Quick) Profile::Push(Profile::Var_RawInteractableMean, 50);
            bool Result = Inventory::Open(Profile::RollUseGametabHotKey()) && Pot.Interact("Drink");
            if (Quick) Profile::Pop();
            return Boosts::Has(P, 0);
        };

        return WaitFunc(2000, 100, DoDrink, true);
    } else
    {
        if (Quick) Profile::Push(Profile::Var_RawInteractableMean, 50);
        bool Result = Inventory::Open(Profile::RollUseGametabHotKey()) && Pot.Interact("Drink");
        if (Quick) Profile::Pop();

        auto HasDrink = [&]() -> bool { return Boosts::Has(P, 0); };
        return Result && WaitFunc(1250, 50, HasDrink, true);
    }
}

std::int32_t Boosts::CountPotions(const Boosts::POTION& P, std::int32_t Stack)
{
    if (Boosts::Potions.count(P))
    {
        if (Stack <= -1)
        {
            std::vector<std::string> DegradeableNames;
            for (std::uint32_t I = 1; I <= Potions[P].StacksTo; I++)
                DegradeableNames.emplace_back(Potions[P].Name + std::to_string(I) + ")");

            std::int32_t Result = 0;
            const auto InvNames = Inventory::GetItemNames();
            for (const auto& N : InvNames)
            {
                if (N.find(Potions[P].Name) != std::string::npos)
                {
                    const auto Pos = N.find('(');
                    if (Pos != std::string::npos)
                        Result += std::stoi(N.substr(Pos + 1));
                }
            }
            return Result;
        } else
            return Inventory::Count(Boosts::Potions[P].Name + std::to_string(Stack) + ")");
    }
    return 0;
}

bool Boosts::Has(const Boosts::POTION& P, std::uint32_t Tolerence)
{
    if (Boosts::Potions.count(P))
    {
        bool Has = false;
        for (const auto& S : Boosts::Potions[P].Skills)
        {
            const auto Current = Stats::GetCurrentLevel(S);
            const auto Real = Stats::GetRealLevel(S);

            if (Current > Real + Tolerence)
                Has = true;

            if (!Has) return false;
        }
        return true;
    }
    return false;
}

Interactable::Item Boosts::GetPotion(const POTION& P)
{
    if (Boosts::Potions.count(P))
    {
        std::vector<std::string> Names;
        for (std::uint32_t I = 1; I <= Boosts::Potions[P].StacksTo; I++)
            Names.emplace_back(Boosts::Potions[P].Name + std::to_string(I) + ")");
        return Inventory::GetItem(Names);
    }
    return Interactable::Item();
}

std::vector<std::tuple<std::string, std::uint32_t, std::uint32_t>> Boosts::GetStatus(const Boosts::POTION& P)
{
    std::vector<std::tuple<std::string, std::uint32_t, std::uint32_t>> Result;
    if (Boosts::Potions.count(P))
        for (const auto& S : Boosts::Potions[P].Skills)
            Result.emplace_back(std::make_tuple(SkillNames[S], Stats::GetCurrentLevel(S), Stats::GetRealLevel(S)));

    return Result;
}
