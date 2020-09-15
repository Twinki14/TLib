#include "../TScript.hpp"
#include "../../Include/Utilities/Runes.hpp"
#include <Game/Interfaces/GameTabs/Inventory.hpp>
#include <Game/Interfaces/GameTabs/Equipment.hpp>
#include <Game/Tools/Settings.hpp>
#include <set>
#include <algorithm>

namespace Runes
{
    namespace
    {
        std::vector<std::int32_t> GetProvidingIDs(const std::vector<Runes::RUNE>& Runes, std::int32_t Source)
        {
            std::set<std::int32_t> RuneIDs;
            for (const auto& R : Runes)
            {
                if (Globals::RuneDefs.count(R))
                {
                    for (const auto& Provider : Globals::RuneDefs.at(R).Providers)
                        if (Provider.Source & Source)
                            RuneIDs.emplace(Provider.ItemID);
                }
            }
            return std::vector<std::int32_t>(RuneIDs.begin(), RuneIDs.end());
        }

        std::vector<Globals::RuneProviderDef> GetProvidingDefs(const std::vector<Runes::RUNE>& Runes, std::int32_t Source)
        {
            std::map<std::int32_t, Globals::RuneProviderDef> Providers;
            for (const auto& R : Runes)
            {
                if (Globals::RuneDefs.count(R))
                {
                    auto DefProviders = Globals::RuneDefs.at(R).Providers;
                    for (auto& DefProvider : DefProviders)
                        if (DefProvider.Source & Source)
                            Providers.try_emplace(DefProvider.ItemID, std::move(DefProvider));
                }
            }

            std::vector<Globals::RuneProviderDef> Result;
            Result.reserve(Providers.size());
            for (auto& [ID, Ref] : Providers)
                Result.emplace_back(std::move(Ref));
            return Result;
        }

        std::vector<std::pair<std::int32_t, std::int32_t>> GetRunePouchRunes()
        {
            static const std::vector<std::int32_t> RuneIDs = { 556, 555, 557, 554, 558, 562, 560, 565, 564, 561, 563, 559, 566, 9075, 4695, 4698, 4696, 4699, 4694, 4697, 21880 };

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

            const RunePouchUnion Runes = { .Raw = ((((std::uint64_t) Settings::GetSetting(1140)) << 32) | (std::uint32_t) Settings::GetSetting(1139)) };
            return {
                    std::make_pair((Runes.RuneOne > 0 && Runes.RuneOne < RuneIDs.size() ? RuneIDs[Runes.RuneOne - 1] : -1), Runes.RuneOneAmount),
                    std::make_pair((Runes.RuneTwo > 0 && Runes.RuneTwo < RuneIDs.size() ? RuneIDs[Runes.RuneTwo - 1] : -1), Runes.RuneTwoAmount),
                    std::make_pair((Runes.RuneThree > 0 && Runes.RuneThree < RuneIDs.size() ? RuneIDs[Runes.RuneThree - 1] : -1), Runes.RuneThreeAmount),
            };
        }
    }

    std::int32_t Count(Runes::RUNE Rune)
    {
        return Runes::Count(Rune, Runes::INVENTORY | Runes::POUCH | Runes::EQUIPMENT);
    }

    std::int32_t Count(Runes::RUNE Rune, std::int32_t Source)
    {
        std::int32_t C = 0;
        if (Source & Runes::INVENTORY || Source & Runes::POUCH)
        {
            auto RuneIDs = GetProvidingIDs({ Rune }, Source);

            if (Source & Runes::INVENTORY && !RuneIDs.empty())
            {
                auto InventoryIDs = Inventory::GetItemIDs();
                auto InventoryAmounts = Inventory::GetItemAmounts();
                for (std::uint32_t I = 0; I < InventoryIDs.size(); I++)
                    if (std::any_of(RuneIDs.begin(), RuneIDs.end(), [&InventoryIDs, &I](const std::int32_t& A) -> bool { return A == InventoryIDs[I]; }))
                        C += InventoryAmounts[I];
            }

            if (Source & Runes::POUCH)
            {
                auto PouchRunes = GetRunePouchRunes();
                for (const auto& PouchRune : PouchRunes)
                    if (std::any_of(RuneIDs.begin(), RuneIDs.end(), [&PouchRune](const std::int32_t& A) -> bool { return A == PouchRune.first; }))
                        C += PouchRune.second;
            }
        }

        if (Source & Runes::EQUIPMENT)
        {
            auto EquipmentIDs = Equipment::GetItemIDs();
            auto ProvidingDefs = GetProvidingDefs({ Rune }, Runes::EQUIPMENT);

            for (const auto& ID : EquipmentIDs)
            {
                for (const auto& Provider : ProvidingDefs)
                    if (Provider.ItemID == ID)
                    {
                        if (Provider.ProvidedAmount == -1) return -1; // full stop, we have infinite runes
                        C += Provider.ProvidedAmount;
                    }
            }
        }

        return C;
    }

    bool Contains(Runes::RUNE Rune)
    {
        auto Count = Runes::Count(Rune, Runes::INVENTORY | Runes::POUCH | Runes::EQUIPMENT);
        return Count > 0 || Count == -1;
    }

    bool Contains(Runes::RUNE Rune, std::int32_t Source)
    {
        auto Count = Runes::Count(Rune, Source);
        return Count > 0 || Count == -1;
    }

    bool Contains(const std::vector<Runes::RUNE>& Runes)
    {
        for (const auto& Rune : Runes)
            if (!Runes::Contains(Rune))
                return false;
        return true;
    }

    bool Contains(const std::vector<Runes::RUNE>& Runes, std::int32_t Source)
    {
        const auto InventoryIDs = Source & Runes::INVENTORY ? Inventory::GetItemIDs() : std::vector<std::int32_t>();
        const auto PouchIDs = Source & Runes::POUCH ? Runes::Pouch::GetRuneIDs() : std::vector<std::int32_t>();
        const auto EquipmentIDs = Source & Runes::EQUIPMENT ? Equipment::GetItemIDs() : std::vector<std::int32_t>();

        const auto ProvidingRuneIDs = (Source & Runes::INVENTORY || Source & Runes::POUCH) ? GetProvidingIDs(Runes, Source) : std::vector<std::int32_t>();
        const auto EquipmentProviderDefs = Source & Runes::EQUIPMENT ? GetProvidingDefs(Runes, Source) :  std::vector<Globals::RuneProviderDef>();

        for (const auto& Rune : Runes)
        {
            const auto ProvidingDefs = GetProvidingDefs( { Rune }, Source);

            bool Found = false;
            for (const auto& Provider : ProvidingDefs)
            {
                if (!Found && Provider.Source & Runes::INVENTORY && Source & Runes::INVENTORY)
                {
                    if (std::any_of(InventoryIDs.begin(), InventoryIDs.end(), [&Provider](const std::int32_t& InventoryID) -> bool { return Provider.ItemID == InventoryID; } ))
                    {
                        Found = true;
                        break;
                    }
                }

                if (!Found && Provider.Source & Runes::POUCH && Source & Runes::POUCH)
                {
                    if (std::any_of(PouchIDs.begin(), PouchIDs.end(), [&Provider](const std::int32_t& PouchID) -> bool { return Provider.ItemID == PouchID; } ))
                    {
                        Found = true;
                        break;
                    }
                }

                if (!Found && Provider.Source & Runes::EQUIPMENT && Source & Runes::EQUIPMENT)
                {
                    if (std::any_of(EquipmentIDs.begin(), EquipmentIDs.end(), [&Provider](const std::int32_t& EquipID) -> bool { return Provider.ItemID == EquipID; } ))
                    {
                        Found = true;
                        break;
                    }
                }
            }
            if (!Found) return false;
        }
        return true;
    }

    namespace Pouch
    {
        std::vector<Runes::RUNE> GetRunes()
        {
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

            std::vector<Runes::RUNE> Result;
            const RunePouchUnion Runes = { .Raw = ((((std::uint64_t) Settings::GetSetting(1140)) << 32) | (std::uint32_t) Settings::GetSetting(1139)) };
            if (Runes.RuneOne > 0) Result.emplace_back((Runes::RUNE) Runes.RuneOne);
            if (Runes.RuneTwo > 0) Result.emplace_back((Runes::RUNE) Runes.RuneTwo);
            if (Runes.RuneThree > 0) Result.emplace_back((Runes::RUNE) Runes.RuneThree);
            return Result;
        }

        std::vector<std::int32_t> GetRuneIDs()
        {
            auto Runes = GetRunePouchRunes();
            std::vector<std::int32_t> IDs;
            auto It = Runes.begin();
            while (It != Runes.end())
            {
                IDs.emplace_back(It->first);
                It = Runes.erase(It);
            }
            return IDs;
        }

        std::vector<std::int32_t> GetRuneAmounts()
        {
            auto Runes = GetRunePouchRunes();
            std::vector<std::int32_t> Amounts;
            auto It = Runes.begin();
            while (It != Runes.end())
            {
                Amounts.emplace_back(It->second);
                It = Runes.erase(It);
            }
            return Amounts;
        }

        std::int32_t Count(RUNE Rune)
        {
            auto IDs = Pouch::GetRuneIDs();
            auto Amounts = Pouch::GetRuneAmounts();
            auto RuneIDs = GetProvidingIDs({ Rune }, Runes::POUCH);

            if (IDs.size() != Amounts.size())
                return 0;

            std::int32_t C = 0;
            for (std::uint32_t I = 0; I < IDs.size(); I++)
            {
                auto& ID = IDs[I];
                for (const auto& RID : RuneIDs)
                    if (RID == ID) C += Amounts[I];
            }
            return C;
        }
    }
}


