#ifndef TLIB_EXTENSIONS_HPP
#define TLIB_EXTENSIONS_HPP

#include <Core/Globals.hpp>
#include <Core/Types/Point.hpp>
#include <Core/Paint.hpp>

namespace Globals
{
    const SettingsDef SETTING_POH_LOCATION      = { 738, 0, 0xF };
    const SettingsDef SETTING_CURRENT_SPELLBOOK = { 439, 0, 0xF };
}

namespace Settings
{
    typedef enum POH_LOCATION
    {
        RIMMINGTON      = 1,
        TAVERLEY        = 2,
        POLLNIVNEACH    = 3,
        RELLEKKA        = 4,
        BRIMHAVEN       = 5,
        YANILLE         = 6,
        HOSIDIUS        = 8
    } POH_LOCATION;

    POH_LOCATION GetPOHLocation();

    typedef enum SPELLBOOK
    {
        NORMAL,
        ANCIENTS,
        LUNARS,
        ARCEUUS
    } SPELLBOOK;

    SPELLBOOK GetCurrentSpellbook();
}

namespace Paint
{
    void DrawFancyDestination(const Tile& T, double DestPointRadius, double PlayerPointRadius, const Paint::Pixel& DestColor, const Paint::Pixel& DotLineColor);
}

#endif //TLIB_EXTENSIONS_HPP
