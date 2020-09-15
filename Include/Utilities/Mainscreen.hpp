#ifndef TLIB_MAINSCREEN_HPP_INCLUDED
#define TLIB_MAINSCREEN_HPP_INCLUDED

#include <Core/Globals.hpp>
#include <Game/Interfaces/Mainscreen.hpp>
#include <Core/Types/Convex.hpp>

namespace Globals
{
    typedef enum ANGLE_DIRECTION
    {
        NORTH,
        NORTH_EAST,
        EAST,
        SOUTH_EAST,
        SOUTH,
        SOUTH_WEST,
        WEST,
        NORTH_WEST
    } ANGLE_DIRECTION;
}

namespace Mainscreen
{
    Globals::ANGLE_DIRECTION GetFacingDirection();
    Globals::ANGLE_DIRECTION GetTileDirection(const Tile& T);

    bool IsFacing(const Tile& T);
    bool IsTileVisible(const Tile& T);

    Tile GetTrueLocation();
    Tile GetTileInFront();
    double GetProjectedDistance(const Tile& From, const Tile& To);
    double GetProjectedDistance(const Tile& T, bool UseMiddle = false, Point* PlayerModelPointOut = nullptr);

    Box GetTileBox(const Tile& T);
    Convex GetTileConvex(const Tile& T);

    bool ClickTileEx(const Tile& T, bool CheckUptext = false, const std::function<bool()>& Interrupt = nullptr);

    Tile GetHoveringTile();
    Tile GetHoveringTile(Point& P);
}

#endif // TLIB_MAINSCREEN_HPP_INCLUDED