#include "../../Include/Utilities/Extensions.hpp"
#include "../../Include/Utilities/Mainscreen.hpp"
#include <Game/Tools/Settings.hpp>
#include <Core/Internal.hpp>

namespace Settings
{
    POH_LOCATION GetPOHLocation()
    {
        std::int32_t Setting = Settings::GetSetting(Globals::SETTING_POH_LOCATION.Index);
        std::uint32_t Bits = (Setting >> Globals::SETTING_POH_LOCATION.BitPos) & Globals::SETTING_POH_LOCATION.BitMask;
        return (POH_LOCATION) Bits;
    }

    SPELLBOOK GetCurrentSpellbook()
    {
        std::int32_t Setting = Settings::GetSetting(Globals::SETTING_CURRENT_SPELLBOOK.Index);
        std::uint32_t Bits = (Setting >> Globals::SETTING_CURRENT_SPELLBOOK.BitPos) & Globals::SETTING_CURRENT_SPELLBOOK.BitMask;
        return (SPELLBOOK) Bits;
    }
}

namespace Paint
{
    void DrawFancyDestination(const Tile& T, double DestPointRadius, double PlayerPointRadius, const Paint::Pixel& DestColor, const Paint::Pixel& DotLineColor)
    {
        Point PlayerModelPoint;
        const auto Dist = Mainscreen::GetProjectedDistance(T, true, &PlayerModelPoint);
        const auto TileMS = Internal::TileToMainscreen(T, 0, 0, 0);
        Paint::DrawTile(T, DestColor.Red, DestColor.Green, DestColor.Blue, DestColor.Alpha);
        Paint::DrawLine(PlayerModelPoint, TileMS, DotLineColor.Red, DotLineColor.Green, DotLineColor.Blue, DotLineColor.Alpha);
        Paint::DrawDot(PlayerModelPoint, PlayerPointRadius, DotLineColor.Red, DotLineColor.Green, DotLineColor.Blue, DotLineColor.Alpha);
        Paint::DrawDot(TileMS, DestPointRadius, DotLineColor.Red, DotLineColor.Green, DotLineColor.Blue, DotLineColor.Alpha);
        Paint::DrawString(std::to_string(Dist), TileMS, DotLineColor.Red, DotLineColor.Green, DotLineColor.Blue, DotLineColor.Alpha);
    }
}