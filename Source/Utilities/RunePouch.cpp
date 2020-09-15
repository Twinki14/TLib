#include "../../Include/Utilities/RunePouch.hpp"
#include <Game/Tools/Settings.hpp>
#include <Core/Internal.hpp>

// Ordered by RunePouch::RUNES enum, RUNES - 1
std::vector<std::uint32_t> RuneIDs = { 556, 555, 557, 554, 558, 562, 560, 565, 564, 561, 563, 559, 566, 9075, 4695, 4698, 4696, 4699, 4694, 4697, 21880 };

union RunePouchUnion
{
    std::uint64_t Raw;
    struct
    {
        unsigned int RuneOne : 6;
        unsigned int RuneTwo : 6;
        unsigned int RuneThree : 6;
        unsigned int RuneOneAmount : 14;
        unsigned int RuneTwoAmount : 14;
        unsigned int RuneThreeAmount : 14;
        unsigned int Pad : 4;
    };
};

std::array<RunePouch::Rune, 3> RunePouch::GetRunes()
{
    RunePouchUnion Runes = { .Raw = ((((std::uint64_t) Settings::GetSetting(1140)) << 32) | (std::uint32_t) Settings::GetSetting(1139)) };

    RunePouch::Rune RuneOne     = { (RunePouch::RUNES) Runes.RuneOne, ((Runes.RuneOne > 0 && Runes.RuneOne < RuneIDs.size()) ? RuneIDs[Runes.RuneOne - 1] : -1), "", Runes.RuneOneAmount };
    RunePouch::Rune RuneTwo     = { (RunePouch::RUNES) Runes.RuneTwo, ((Runes.RuneTwo > 0 && Runes.RuneTwo < RuneIDs.size()) ? RuneIDs[Runes.RuneTwo - 1] : -1), "", Runes.RuneTwoAmount };
    RunePouch::Rune RuneThree   = { (RunePouch::RUNES) Runes.RuneThree, ((Runes.RuneThree > 0 && Runes.RuneThree < RuneIDs.size()) ? RuneIDs[Runes.RuneThree - 1] : -1), "", Runes.RuneThreeAmount };

    std::array<RunePouch::Rune, 3> Result = { std::move(RuneOne), std::move(RuneTwo), std::move(RuneThree) };
    for (auto R : Result)
    {
        if (R.ID != -1)
        {
            Internal::ItemInfo Info = Internal::GetItemInfo(R.ID);
            if (Info)
                R.Name = Info.GetName();
            else
                R.Name = Internal::GetItemName(R.ID);
        }
    }

    return Result;
}

std::int32_t RunePouch::Count(const RunePouch::RUNES& Rune)
{
    const auto Runes = RunePouch::GetRunes();
    for (const auto& R : Runes)
        if (R.Rune == Rune)
            return R.Amount;
    return 0;
}

bool RunePouch::Contains(const RunePouch::RUNES& Rune, uint32_t Tolerence)
{
    const auto Runes = RunePouch::GetRunes();
    for (const auto& R : Runes)
        if (R.Rune == Rune)
            return R.Amount >= Tolerence;
    return false;
}

bool RunePouch::Contains(const std::vector<RunePouch::RUNES>& Runes, uint32_t Tolerence)
{
    const auto PouchRunes = RunePouch::GetRunes();

    for (const auto& PR : PouchRunes)
    {
        bool Found = false;
        for (const auto& R : Runes)
        {
            if (PR.Rune == R)
            {
                Found = true;
                if (PR.Amount < Tolerence)
                    return false;
            }
        }

        if (!Found) return false;
    }

    return true;
}
