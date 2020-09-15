#ifndef TLIB_PRAYER_HPP_INCLUDED
#define TLIB_PRAYER_HPP_INCLUDED

#include <Core/Globals.hpp>
#include <Game/Interfaces/GameTabs/Prayer.hpp>

namespace Globals
{
    const SettingsDef SETTING_QUICK_PRAYER_PRAYERS = { 84 }; // To be used with PrayerDefs
    const SettingsDef SETTING_QUICK_PRAYER_TOGGLE = { 375, 0 }; // Toggle that gets activate by clicking the prayer icon near the minimap
}

namespace Prayer
{
    bool IsAnyActive();
    bool IsQuickPrayersActive();
    bool IsQuickPrayerToggleActive();
    bool IsOnlyQuickPrayersActive();

    std::vector<Prayer::PRAYERS> GetActive();
    std::vector<Prayer::PRAYERS> GetQuickPrayers();
    Prayer::PRAYERS GetActiveProtection();

    bool Disable();
    bool DisableIndividually();
    bool DisableProtections();

    bool QuickPrayer();
    bool QuickDrinkRestore();
}

#endif // TLIB_PRAYER_HPP_INCLUDED