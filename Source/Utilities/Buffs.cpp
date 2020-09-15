#include "../../Include/Utilities/Buffs.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include <Core/Internal.hpp>
#include <Game/Interfaces/Chat.hpp>
#include <Game/Interfaces/GameTabs/Combat.hpp>
#include <Game/Interfaces/GameTabs/Inventory.hpp>

#ifndef MINUTE
    #define MINUTE 60000
#endif

std::map<std::uint32_t, Buffs::PotionBuffDefinition> Buffs::Potions =
{
    { EXTENDED_SUPER_ANTIFIRE, { "Extended super antifire(", "You drink some of your extended super antifire potion.", 6 * MINUTE, false, 4 } },
    { ANTI_VENOM_PLUS, { "Anti-venom+(", "You drink some of your super antivenom potion", 3 * MINUTE, true, 4 } },

    { DIVINE_MAGIC, { "Divine magic potion(", "You drink some of your divine magic potion.", 5 * MINUTE, true, 4, 10 } },
    { DIVINE_RANGING, { "Divine ranging potion(", "You drink some of your divine ranging potion.", 5 * MINUTE, true, 4, 10 } },
    { DIVINE_SUPER_ATTACK, { "Divine super attack potion(", "You drink some of your divine super attack potion.", 5 * MINUTE, true, 4, 10 } },
    { DIVINE_SUPER_COMBAT, { "Divine super combat potion(", "You drink some of your divine combat potion.", 5 * MINUTE, true, 4, 10 } },
    { DIVINE_SUPER_DEFENCE, { "Divine super defence potion(", "You drink some of your divine super defence potion.", 5 * MINUTE, true, 4, 10 } },
    { DIVINE_SUPER_STRENGTH, { "Divine super strength potion(", "You drink some of your divine super strength potion.", 5 * MINUTE, true, 4, 10 } },
};

void Buffs::Scan()
{
    static const std::vector<Chat::CHAT_TYPE> Types = { (Chat::CHAT_TYPE) 0, (Chat::CHAT_TYPE) 105 };
    const auto Messages = Chat::GetMessages(Types, false);
    if (Messages.empty()) return;

    for (auto& [ID, Potion] : Buffs::Potions)
    {
        if (Potion.CountdownStarted)
        {
            if (Potion.C.IsFinished())
            {
                Potion.CountdownStarted = false;
                Potion.C = Countdown();
            }
        }

        std::uint64_t DrinkMessageTick = 0;
        std::uint64_t DeadMessageTick = 0;

        for (const auto& M : Messages)
        {
            const auto Text = M.GetText();
            const auto Tick = M.GetTick();

            if (Text == Potion.DrinkMessage)
            {
                if (Tick > DrinkMessageTick)
                    DrinkMessageTick = Tick;
            } else if (Potion.RemovedOnDeath && Text == "Oh dear, you are dead!")
            {
                if (Tick > DeadMessageTick)
                    DeadMessageTick = Tick;
            }
        }

        if (DrinkMessageTick > 0 && DrinkMessageTick > Potion.LastDrinkTick && DeadMessageTick < DrinkMessageTick)
        {
            std::uint64_t MSSinceDrink = ((Internal::GetGameTick() - DrinkMessageTick) * 20);
            if (MSSinceDrink > Potion.Duration) continue;

            Potion.LastDrinkTick = DrinkMessageTick;
            Potion.C = Countdown(Potion.Duration - MSSinceDrink);
            Potion.CountdownStarted = true;
        }
    }
}

Interactable::Item Buffs::GetPotion(const Buffs::Potion& P)
{
    if (Buffs::Potions.count(P))
    {
        std::vector<std::string> Names;
        for (std::uint32_t I = 1; I <= Buffs::Potions[P].StacksUpTo; I++)
            Names.emplace_back(Buffs::Potions[P].Name + std::to_string(I) + ")");
        return Inventory::GetItem(Names);
    }
    return Interactable::Item();
}

std::int32_t Buffs::CountPotions(const Buffs::Potion& P, int32_t Stack)
{
    if (Buffs::Potions.count(P))
    {
        if (Stack <= -1)
        {
            std::vector<std::string> DegradeableNames;
            for (std::uint32_t I = 1; I <= Potions[P].StacksUpTo; I++)
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
            return Inventory::Count(Potions[P].Name + std::to_string(Stack) + ")");
    }
    return 0;
}

bool Buffs::CanDrink(const Buffs::Potion& P, std::uint64_t Tolerence)
{
    if (Buffs::Potions.count(P))
    {
        if (Combat::GetHealth() < Buffs::Potions[P].HealthDamage) return false;
        if (!Buffs::Potions[P].CountdownStarted) return true;
        if (Buffs::Potions[P].C.GetTimeRemaining() <= Tolerence) return true;
    }
    return false;
}

std::uint64_t Buffs::GetTimeLeft(const Buffs::Potion& P)
{
    if (Buffs::Potions.count(P))
    {
        if (!Buffs::Potions[P].CountdownStarted) return 0;
        return Buffs::Potions[P].C.GetTimeRemaining();
    }
    return 0;
}

bool Buffs::Drink(const Buffs::Potion& P, bool Quick, bool Spam)
{
    const auto Pot = Buffs::GetPotion(P);
    if (!Pot) return false;

    if (Spam)
    {
        auto DoDrink = [&]() -> bool
        {
            if (Buffs::GetPercentageLeft(P) >= 0.95) return true;
            if (Quick) Profile::Push(Profile::Var_RawInteractableMean, 50);
            bool Result = Inventory::Open(Profile::RollUseGametabHotKey()) && Pot.Interact("Drink");
            if (Quick) Profile::Pop();
            return Buffs::GetPercentageLeft(P) >= 0.95;
        };

        return WaitFunc(2000, 100, DoDrink, true);
    } else
    {
        if (Quick) Profile::Push(Profile::Var_RawInteractableMean, 50);
        bool Result = Inventory::Open(Profile::RollUseGametabHotKey()) && Pot.Interact("Drink");
        if (Quick) Profile::Pop();
        if (Quick) return Result;

        auto HasDrink = [&]() -> bool { return Buffs::GetPercentageLeft(P) >= 0.95; };
        return Result && WaitFunc(450, 25, HasDrink, true);
    }
}

double Buffs::GetPercentageLeft(const Buffs::Potion& P)
{
    if (Buffs::Potions.count(P))
        return (double) Buffs::GetTimeLeft(P) / Buffs::Potions[P].Duration;
    return 0;
}

void Buffs::OnDeath()
{
    for (auto&[ID, Potion] : Buffs::Potions)
    {
        if (Potion.RemovedOnDeath)
        {
            Potion.CountdownStarted = false;
            Potion.C = Countdown();
        }
    }
}

Buffs::PotionBuffDefinition Buffs::GetPotionBuffDefinition(const Potion &P)
{
    return (Buffs::Potions.count(P)) ? (Buffs::Potions[P]) : (Buffs::PotionBuffDefinition{});
}

