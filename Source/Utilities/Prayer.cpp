#include "../../Include/Utilities/Prayer.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include "../../Include/TScript.hpp"
#include <Game/Interfaces/Minimap.hpp>
#include <Game/Tools/Settings.hpp>
#include <Core/Time.hpp>
#include <Game/Interfaces/GameTabs/Inventory.hpp>
#include <Core/Math.hpp>

namespace Prayer
{
    bool IsAnyActive()
    {
        return Settings::GetSetting(Globals::SETTING_CURRENT_PRAYER.Index) != 0;
    }

    bool IsQuickPrayersActive()
    {
        const auto Prayers = Prayer::GetQuickPrayers();
        if (Prayers.empty()) return false;
        for (const auto& P : Prayers)
            if (!Prayer::IsActive(P))
                return false;
        return true;
    }

    bool IsQuickPrayerToggleActive()
    {
        return Settings::GetSettingBit(Globals::SETTING_QUICK_PRAYER_TOGGLE.Index, Globals::SETTING_QUICK_PRAYER_TOGGLE.BitPos);
    }

    bool IsOnlyQuickPrayersActive()
    {
        const auto QuickPrayers = Prayer::GetQuickPrayers();
        const auto ActivePrayers = Prayer::GetActive();
        if (QuickPrayers.empty()) return false;
        if (ActivePrayers.empty()) return false;
        if (QuickPrayers.size() != ActivePrayers.size()) return false;

        for (const auto& P : QuickPrayers)
            if (std::find(ActivePrayers.begin(), ActivePrayers.end(), P) == ActivePrayers.end())
                return false;
        return true;
    }

    std::vector<Prayer::PRAYERS> GetActive()
    {
        std::vector<Prayer::PRAYERS> Result;
        for (std::int32_t P = Prayer::THICK_SKIN; P < Prayer::AUGURY; P++)
            if (Prayer::IsActive((Prayer::PRAYERS) P))
                Result.emplace_back((Prayer::PRAYERS) P);
        return Result;
    }

    std::vector<Prayer::PRAYERS> GetQuickPrayers()
    {
        std::vector<Prayer::PRAYERS> Result;
        for (std::uint32_t I = 0; I < Globals::GameTabs::PrayerDefs.size(); I++)
            if (Settings::GetSettingBit(Globals::SETTING_QUICK_PRAYER_PRAYERS.Index, Globals::GameTabs::PrayerDefs[I].BitPos))
                Result.emplace_back((Prayer::PRAYERS) I);
        return Result;
    }

    Prayer::PRAYERS GetActiveProtection()
    {
        if (Prayer::IsActive(Prayer::PROTECT_FROM_MAGIC)) return Prayer::PROTECT_FROM_MAGIC;
        if (Prayer::IsActive(Prayer::PROTECT_FROM_MISSILES)) return Prayer::PROTECT_FROM_MISSILES;
        if (Prayer::IsActive(Prayer::PROTECT_FROM_MELEE)) return Prayer::PROTECT_FROM_MELEE;
        return (Prayer::PRAYERS) -1;
    }

    bool Disable()
    {
        if (Prayer::IsQuickPrayerToggleActive())
            return Minimap::ClickQuickPrayer() && WaitFunc(850, 50, Prayer::IsAnyActive, false);
        else
            return Prayer::DisableIndividually();
        return false;
    }

    bool DisableIndividually()
    {
        if (!Prayer::IsAnyActive()) return true;
        for (std::int32_t P = Prayer::THICK_SKIN; P < Prayer::AUGURY; P++)
            if (!Prayer::Deactivate((Prayer::PRAYERS) P))
                return false;
        return WaitFunc(850, 50, Prayer::IsAnyActive, false);
    }

    bool DisableProtections()
    {
        static const auto AllDisabled = []() -> bool
        {
            return !Prayer::IsActive(Prayer::PROTECT_FROM_MAGIC) &&
                   !Prayer::IsActive(Prayer::PROTECT_FROM_MISSILES) &&
                   !Prayer::IsActive(Prayer::PROTECT_FROM_MELEE);
        };

        if (!AllDisabled()) Prayer::Open(UniformRandom() <= 0.75);

        return Prayer::Deactivate(Prayer::PROTECT_FROM_MAGIC) &&
               Prayer::Deactivate(Prayer::PROTECT_FROM_MISSILES) &&
               Prayer::Deactivate(Prayer::PROTECT_FROM_MELEE) && WaitFunc(150, 15, AllDisabled, true);
    }

    bool QuickPrayer()
    {
        const auto Prayers = Prayer::GetQuickPrayers();
        if (Prayers.empty()) return false;

        if (!Prayer::IsQuickPrayersActive() && Prayer::GetPoints() > 0)
            if (Minimap::ClickQuickPrayer())
                return WaitFunc(650, 50, Prayer::IsQuickPrayersActive, true);
        return true;
    }

    bool QuickDrinkRestore()
    {
        static const auto Pots = std::vector<std::string> { "Super restore(1)", "Prayer potion(1)",
                                                            "Super restore(2)", "Prayer potion(2)",
                                                            "Super restore(3)", "Prayer potion(3)",
                                                            "Super restore(4)", "Prayer potion(4)" };
        auto Pot = Inventory::GetItem(Pots);
        if (!Pot)
        {
            DebugLog("nope");
            return false;
        }

        Profile::Push(Profile::Var_RawInteractableMean, 0);
        Profile::Push(Profile::Var_RawMoveMean, 95);
        Profile::Push(Profile::Var_RawMouseUpMean, 0);
        bool Clicked = Inventory::Open(Profile::RollUseGametabHotKey()) && Pot.Interact("Drink");
        Profile::Pop(3);
        return Clicked;
    }
}