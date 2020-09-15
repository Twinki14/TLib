#include "../../Include/Utilities/Mainscreen.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include "../../Include/TScript.hpp"
#include <Core/Internal.hpp>
#include <Game/Interfaces/Minimap.hpp>
#include <Game/Tools/Settings.hpp>
#include <Game/Tools/Interact.hpp>
#include <Game/Tools/Profile.hpp>
#include <Game/Tools/Camera.hpp>

namespace Mainscreen
{
    Globals::ANGLE_DIRECTION GetFacingDirection()
    {
        const auto Local = Internal::GetLocalPlayer();
        if (!Local) return Globals::SOUTH;

        const auto Angle = Local.GetAngle();

        enum ACTOR_ANGLE_DIRECTION
        {
            SOUTH = 0,
            SOUTH_WEST = 256,
            WEST = 512,
            NORTH_WEST = 768,
            NORTH = 1024,
            NORTH_EAST = 1280,
            EAST = 1536,
            SOUTH_EAST = 1792,
        };

        switch (Angle)
        {
            case NORTH_WEST + 1 ... NORTH: return Globals::NORTH;
            case NORTH + 1 ... NORTH_EAST: return Globals::NORTH_EAST;
            case NORTH_EAST + 1 ... EAST: return Globals::EAST;
            case EAST + 1 ... SOUTH_EAST: return Globals::SOUTH_EAST;
            case SOUTH:
            case SOUTH_EAST + 1 ... 2016: return Globals::SOUTH;
            case SOUTH + 1 ... SOUTH_WEST: return Globals::SOUTH_WEST;
            case SOUTH_WEST + 1 ... WEST: return Globals::WEST;
            case WEST + 1 ... NORTH_WEST: return Globals::NORTH_WEST;
            default: break;
        }

        return Globals::SOUTH;
    }

    Globals::ANGLE_DIRECTION GetTileDirection(const Tile& T)
    {
        const auto Pos = Minimap::GetPosition();
        if (!Pos) return Globals::SOUTH;

        const auto Diff = T - Pos;
        if (Diff.X == 0 && Diff.Y > 0) return Globals::NORTH;
        if (Diff.X > 0 && Diff.Y > 0) return Globals::NORTH_EAST;
        if (Diff.X > 0 && Diff.Y == 0) return Globals::EAST;
        if (Diff.X > 0 && Diff.Y < 0) return Globals::SOUTH_EAST;
        if (Diff.X == 0 && Diff.Y < 0) return Globals::SOUTH;
        if (Diff.X < 0 && Diff.Y < 0) return Globals::SOUTH_WEST;
        if (Diff.X < 0 && Diff.Y == 0) return Globals::WEST;
        if (Diff.X < 0 && Diff.Y > 0) return Globals::NORTH_WEST;
        return Globals::SOUTH;
    }

    bool IsFacing(const Tile& T)
    {
        return Mainscreen::GetFacingDirection() == Mainscreen::GetTileDirection(T);
    }

    bool IsTileVisible(const Tile& T)
    {
        const auto Pos = Minimap::GetPosition();
        if (Pos.IsNegative()) return false;

        const auto MSPoint = Internal::TileToMainscreen(T, 0, 0, 0);
        if (!MSPoint.OnScreen()) return false;

        if (T.DistanceFrom(Pos) > 16) return false;

        std::vector<Box> Exclude = Mainscreen::GetBlockingWidgetBoxes();
        for (const auto& Ex : Exclude)
            if (Ex.Contains(MSPoint))
                return false;

        return true;
    }

    Tile GetTrueLocation()
    {
        const auto Local = Internal::GetLocalPlayer();
        if (!Local) return Tile();

        const auto PathX = Local.GetPathX();
        const auto PathY = Local.GetPathY();
        if (PathX.empty() || PathY.empty()) return Tile();

        const auto ClientX = Internal::GetClientX();
        const auto ClientY = Internal::GetClientY();
        const auto ClientPlane = Internal::GetClientPlane();
        if (ClientX < 0 || (ClientX >= 65536) || ClientY < 0 || (ClientY >= 65536))
            return Tile(-1, -1, -1);

        //auto LocalTrueLoc = Tile(PathX[0] * 128 + 128 / 2,  PathY[0] * 128 + 128 / 2, ClientPlane);
        //return Tile(ClientX + (LocalTrueLoc.X >> 7), ClientY + (LocalTrueLoc.Y >> 7), ClientPlane);
        return Tile(ClientX + PathX[0], ClientY + PathY[0], ClientPlane);
    }

    Tile GetTileInFront()
    {
        const auto Pos = Minimap::GetPosition();
        if (!Pos) return Tile();

        switch (Mainscreen::GetFacingDirection())
        {
            case Globals::NORTH: return Pos + Tile(0, 1, 0);
            case Globals::NORTH_EAST: return Pos + Tile(1, 1, 0);
            case Globals::EAST: return Pos + Tile(1, 0, 0);
            case Globals::SOUTH_EAST: return Pos + Tile(1, -1, 0);
            case Globals::SOUTH: return Pos + Tile(0, -1, 0);
            case Globals::SOUTH_WEST: return Pos + Tile(-1, -1, 0);
            case Globals::WEST: return Pos + Tile(-1, 0, 0);
            case Globals::NORTH_WEST: return Pos + Tile(-1, 1, 0);
            default: return Tile();
        }
        return Tile();
    }

    double GetProjectedDistance(const Tile& From, const Tile& To)
    {
        if (From.IsNegative()) return 0.00;
        if (To.IsNegative()) return 0.00;

        const auto ClientX = Internal::GetClientX();
        const auto ClientY = Internal::GetClientY();
        const auto ClientPlane = Internal::GetClientPlane();

        Tile FromWorld = Tile(((From.X - ClientX) << 7), ((From.Y - ClientY) << 7), ClientPlane);
        Tile ToWorld = Tile(((To.X - ClientX) << 7), ((To.Y - ClientY) << 7), ClientPlane);

        return FromWorld.DistanceFrom(ToWorld);
    }

    double GetProjectedDistance(const Tile& T, bool UseMiddle, Point *PlayerModelPointOut)
    {
        if (T.IsNegative()) return 0.00;

        const auto Local = Internal::GetLocalPlayer();
        if (!Local) return 0.00;

        const auto Angle = Local.GetAngle();
        const auto ClientX = Internal::GetClientX();
        const auto ClientY = Internal::GetClientY();
        const auto ClientPlane = Internal::GetClientPlane();

        enum DIRECTION
        {
            SOUTH = 0,
            SOUTH_WEST = 256,
            WEST = 512,
            NORTH_WEST = 768,
            NORTH = 1024,
            NORTH_EAST = 1280,
            EAST = 1536,
            SOUTH_EAST = 1792,
        };

        Tile PlayerWorld = Tile(Local.GetX() - 64, Local.GetY() - 64, ClientPlane);
        Tile TargetWorld = Tile(((T.X - ClientX) << 7), ((T.Y - ClientY) << 7), ClientPlane);

        if (!UseMiddle)
        {
            switch (Angle)
            {
                case SOUTH ... (SOUTH_WEST - 1): PlayerWorld += Tile(0, -32, 0);
                    break;
                case SOUTH_WEST ... (WEST - 1): PlayerWorld += Tile(-32, -32, 0);
                    break;
                case WEST ... (NORTH_WEST - 1): PlayerWorld += Tile(-32, 0, 0);
                    break;
                case NORTH_WEST ... (NORTH - 1): PlayerWorld += Tile(-32, 32, 0);
                    break;
                case NORTH ... (NORTH_EAST - 1): PlayerWorld += Tile(0, 32, 0);
                    break;
                case NORTH_EAST ... (EAST - 1): PlayerWorld += Tile(32, 32, 0);
                    break;
                case EAST ... (SOUTH_EAST - 1): PlayerWorld += Tile(32, 0, 0);
                    break;
                case SOUTH_EAST ... 2016: PlayerWorld += Tile(32, -32, 0);
                    break;
                default: break;
            }
        }

        double Distance = PlayerWorld.DistanceFrom(TargetWorld);

        if (PlayerModelPointOut)
        {
            const auto PT = Tile((Local.GetX() >> 7) + ClientX, (Local.GetY() >> 7) + ClientY, ClientPlane);
            switch (Angle)
            {
                case SOUTH ... (SOUTH_WEST - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, 0, -32, 0);
                    break;
                case SOUTH_WEST ... (WEST - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, -32, -32, 0);
                    break;
                case WEST ... (NORTH_WEST - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, -32, 0, 0);
                    break;
                case NORTH_WEST ... (NORTH - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, -32, 32, 0);
                    break;
                case NORTH ... (NORTH_EAST - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, 0, 32, 0);
                    break;
                case NORTH_EAST ... (EAST - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, 32, 32, 0);
                    break;
                case EAST ... (SOUTH_EAST - 1): *PlayerModelPointOut = Internal::TileToMainscreen(PT, 32, 0, 0);
                    break;
                case SOUTH_EAST ... 2016: *PlayerModelPointOut = Internal::TileToMainscreen(PT, 32, -32, 0);
                    break;
                default: break;
            }

            //Paint::Pixel Color = { 0, 255, 0, 255 };
            //if (Distance <= 545) Color = { 255, 204, 0, 255 };
            // if (Distance <= 289) Color = { 255, 0, 0, 255 };

            //Paint::DrawLine(P, Internal::TileToMainscreen(T, 0, 0, 0), Color.Red, Color.Green, Color.Blue, Color.Alpha);
            //Paint::DrawDot(P, 1.5f, Color.Red, Color.Green, Color.Blue, Color.Alpha);
            //Paint::DrawDot(Internal::TileToMainscreen(T, 0, 0, 0), 1.5f, Color.Red, Color.Green, Color.Blue, Color.Alpha);
        }

        return Distance;
    }

    Box GetTileBox(const Tile& T)
    {
        return Mainscreen::GetTileConvex(T).GetBox();
    }

    Convex GetTileConvex(const Tile& T)
    {
        if (!T) return Convex();

        auto Points = Internal::TileToMainscreenBox(T, 35, 35, 0, 0, 0, 0);
        return Convex(ConvexHull(Points));
    }

    bool ClickTileEx(const Tile& T, bool CheckUptext, const std::function<bool()>& Interrupt)
    {
        if (!T) return false;

        auto TileBox = GetTileBox(T);
        if (!TileBox) return false;

        Profile::Push(Profile::Var_RawMoveMean, 85);
        Profile::Push(Profile::Var_RawMoveDeviation, 0.02);
        Profile::Push(Profile::Var_RawMouseDownMean, 35);
        Profile::Push(Profile::Var_RawMouseDownDeviation, 0.02);
        Profile::Push(Profile::Var_RawMouseUpMean, 0);

        bool Interacted = false;
        auto InteractPoint = TileBox.GetHybridRandomPoint(0.50, 0.50, 0.05, 0.05, 1.00);
        Interacted = CheckUptext ? Interact::Click(InteractPoint, "Walk here") : Interact::Click(InteractPoint);

        if (Interacted)
        {
            Timer DidClickTimer;
            Timer ClickTimer;
            Timer DestTimer;
            auto LastCrosshairState = Mainscreen::GetCrosshairState();
            auto LastDest = Minimap::GetDestination();
            while (true)
            {
                bool Clicked = false;
                bool CrosshairStateValid = LastCrosshairState != Mainscreen::NONE;
                bool DestChanged = LastDest == T;

                if (Terminate)
                {
                    Profile::Pop(5);
                    return false;
                }

                if (!CrosshairStateValid)
                {
                    LastCrosshairState = Mainscreen::GetCrosshairState();
                    CrosshairStateValid = LastCrosshairState != Mainscreen::NONE;
                } else
                {
                    if (!ClickTimer.Paused())
                        DebugLog("Crosshair state not-null after {}ms", ClickTimer.GetTimeElapsed());
                    ClickTimer.Suspend();
                }

                if (!DestChanged)
                {
                    LastDest = Minimap::GetDestination();
                    DestChanged = LastDest == T;
                } else
                {
                    if (!DestTimer.Paused())
                        DebugLog("Destination changed after {}ms", DestTimer.GetTimeElapsed());
                    DestTimer.Suspend();
                }

                if (!CrosshairStateValid && ClickTimer.GetTimeElapsed() >= 65)
                {
                    if (Interact::Click(Internal::TileToMainscreen(T, 0, 0, 0)))
                    {
                        Clicked = true;
                        ClickTimer.Restart();
                        DebugLog("ClickTimer reset");
                    }
                }

                if (!DestChanged && !Clicked && DestTimer.GetTimeElapsed() >= 325)
                {
                    TileBox = GetTileBox(T);
                    if (!TileBox)
                    {
                        DebugLog("Box null");
                        Profile::Pop(5);
                        return false;
                    }

                    if (Interact::Click(TileBox.GetHybridRandomPoint(0.50, 0.50, 0.05, 0.05, 1.00)))
                    {
                        Clicked = true;
                        DestTimer.Restart();
                        DebugLog("DestTimer reset");
                    }
                }

                if (DestChanged && CrosshairStateValid && Mainscreen::IsMoving())
                {
                    DebugLog("Success, DestChanged, CrosshairValid, and moving");
                    Profile::Pop(5);
                    return true;
                }

                if (Mainscreen::GetTrueLocation() == T)
                {
                    DebugLog("Success, TrueLocation matches tile");
                    Profile::Pop(5);
                    return true;
                }

                if (Interrupt && Interrupt())
                {
                    DebugLog("Interrupted");
                    Profile::Pop(5);
                    return false;
                }
            }
        } else
            DebugLog("!Interact > {}", T.IsPositive());
        Profile::Pop(5);
        return false;
    }

    namespace
    {
        std::int32_t GetTileHeight(std::int32_t X, std::int32_t Y, std::int32_t Z,
                                   const std::vector<std::vector<std::vector<std::int32_t>>>& TileHeights,
                                   const std::vector<std::vector<std::vector<std::int8_t>>>& TileSettings)
        {
            std::int32_t LocalX = X >> 7;
            std::int32_t LocalY = Y >> 7;
            if (LocalX >= 0 && LocalY >= 0 && LocalX < 104 && LocalY < 104)
            {
                if ((Z < 3) && ((TileSettings[1][LocalX][LocalY] & 2) == 2))
                    Z++;
                std::int32_t ModX = X & 127;
                std::int32_t ModY = Y & 127;
                std::int32_t I = (ModX * TileHeights[Z][LocalX + 1][LocalY] + (128 - ModX) * TileHeights[Z][LocalX][LocalY]) >> 7;
                std::int32_t J = (TileHeights[Z][LocalX][LocalY + 1] * (128 - ModX) + ModX * TileHeights[Z][LocalX + 1][LocalY + 1]) >> 7;
                return ((128 - ModY) * I + ModY * J) >> 7;
            }
            return 0;
        }

        Point WorldToScreen(std::int32_t X, std::int32_t Y, std::int32_t Z,
                            const std::vector<std::vector<std::vector<std::int32_t>>>& TileHeights,
                            const std::vector<std::vector<std::vector<std::int8_t>>>& TileSettings,
                            std::int32_t CameraX, std::int32_t CameraY, std::int32_t CameraZ,
                            std::int32_t Pitch, std::int32_t Yaw, std::int32_t Scale,
                            std::int32_t ViewportWidth, std::int32_t ViewportHeight,
                            std::int32_t ScreenType, std::int32_t Plane,
                            std::int32_t PositionX, std::int32_t PositionY)
        {

            static const std::vector<std::int32_t> COS_TABLE = [&]
            {
                std::vector<std::int32_t> R;
                for (std::uint32_t I = 0; I < 2048; I++)
                    R.emplace_back((std::int32_t) (65536.0f * std::cos((I * 0.0030679615f))));
                return R;
            }();

            static const std::vector<std::int32_t> SIN_TABLE = [&]
            {
                std::vector<std::int32_t> R;
                for (std::uint32_t I = 0; I < 2048; I++)
                    R.emplace_back((std::int32_t) (65536.0f * std::sin((I * 0.0030679615f))));
                return R;
            }();

            if ((X >= 128) && (Y >= 128) && (X <= 13056) && (Y <= 13056))
            {
                Z = GetTileHeight(PositionX, PositionY, Plane, TileHeights, TileSettings) - Z;
                X -= CameraX;
                Y -= CameraY;
                Z -= CameraZ;
                std::int32_t SinPitch = SIN_TABLE[Pitch];
                std::int32_t CosPitch = COS_TABLE[Pitch];
                std::int32_t SinYaw = SIN_TABLE[Yaw];
                std::int32_t CosYaw = COS_TABLE[Yaw];
                std::int32_t Calculation = (SinYaw * Y + CosYaw * X) >> 16;
                Y = (Y * CosYaw - X * SinYaw) >> 16;
                X = Calculation;
                Calculation = (CosPitch * Z - SinPitch * Y) >> 16;
                Y = (SinPitch * Z + CosPitch * Y) >> 16;
                Z = Calculation;
                if (Y >= 50)
                {
                    std::int32_t ScreenX = X * Scale / Y + (ViewportWidth / 2);
                    std::int32_t ScreenY = Z * Scale / Y + (ViewportHeight / 2);
                    if (ScreenType == 1)
                    {
                        ScreenX += 4;
                        ScreenY += 4;
                    }
                    return Point(ScreenX, ScreenY);
                }
            }
            return Point(-1, -1);
        }

        Point TileToMainscreenEx(const Tile& T, std::int32_t X, std::int32_t Y, std::int32_t Z, const std::vector<std::vector<std::vector<std::int32_t>>>& TileHeights,
                                 const std::vector<std::vector<std::vector<std::int8_t>>>& TileSettings,
                                 std::int32_t CameraX, std::int32_t CameraY, std::int32_t CameraZ,
                                 std::int32_t ClientX, std::int32_t ClientY, std::int32_t ClientPlane,
                                 std::int32_t Pitch, std::int32_t Yaw, std::int32_t Scale,
                                 std::int32_t ViewportWidth, std::int32_t ViewportHeight,
                                 std::int32_t ScreenType)
        {
            X += std::round(((T.X - ClientX) + 0.5f) * 128.0f);
            Y += std::round(((T.Y - ClientY) + 0.5f) * 128.0f);
            if (T.Plane != ClientPlane)
                return Point(-1, -1);
            return WorldToScreen(X, Y, Z, TileHeights, TileSettings, CameraX, CameraY, CameraZ, Pitch, Yaw, Scale, ViewportWidth, ViewportHeight, ScreenType, ClientPlane, X, Y);
        }
    }

    Tile GetHoveringTile()
    {
        Point P;
        return Mainscreen::GetHoveringTile(P);
    }

    Tile GetHoveringTile(Point& OutP)
    {
        Point P = GetMousePos();
        Tile Pos = Minimap::GetPosition();
        auto ClientX = Internal::GetClientX();
        auto ClientY = Internal::GetClientY();
        auto ClientPlane = Internal::GetClientPlane();
        auto TileHeights = Internal::GetTileHeights();
        auto TileSettings = Internal::GetTileSettings();
        auto CameraX = Internal::GetCameraX();
        auto CameraY = Internal::GetCameraY();
        auto CameraZ = Internal::GetCameraZ();
        auto CameraPitch = Internal::GetCameraPitch();
        auto CameraYaw = Internal::GetCameraYaw();
        auto ViewportScale = Internal::GetViewportScale();
        auto ViewportWidth = Internal::GetViewportWidth();
        auto ViewportHeight = Internal::GetViewportHeight();
        auto ScreenType = Settings::GetScreenType();

        std::vector<Point> Vertices;

        for (std::int32_t Y = Pos.Y - 15; Y < Pos.Y + 15; Y++)
            for (std::int32_t X = Pos.X - 15; X < Pos.X + 15; X++)
            {
                Tile T(X, Y, ClientPlane);
                Vertices.clear();
                Vertices.emplace_back(TileToMainscreenEx(T, -64, -64, 0,
                                                         TileHeights, TileSettings,
                                                         CameraX, CameraY, CameraZ,
                                                         ClientX, ClientY, ClientPlane,
                                                         CameraPitch, CameraYaw, ViewportScale,
                                                         ViewportWidth, ViewportHeight, ScreenType));
                Vertices.emplace_back(TileToMainscreenEx(T, 64, -64, 0,
                                                         TileHeights, TileSettings,
                                                         CameraX, CameraY, CameraZ,
                                                         ClientX, ClientY, ClientPlane,
                                                         CameraPitch, CameraYaw, ViewportScale,
                                                         ViewportWidth, ViewportHeight, ScreenType));
                Vertices.emplace_back(TileToMainscreenEx(T, 64, 64, 0,
                                                         TileHeights, TileSettings,
                                                         CameraX, CameraY, CameraZ,
                                                         ClientX, ClientY, ClientPlane,
                                                         CameraPitch, CameraYaw, ViewportScale,
                                                         ViewportWidth, ViewportHeight, ScreenType));
                Vertices.emplace_back(TileToMainscreenEx(T, -64, 64, 0,
                                                         TileHeights, TileSettings,
                                                         CameraX, CameraY, CameraZ,
                                                         ClientX, ClientY, ClientPlane,
                                                         CameraPitch, CameraYaw, ViewportScale,
                                                         ViewportWidth, ViewportHeight, ScreenType));
                Vertices.emplace_back(Vertices[0]);

                Convex C(Vertices);
                if (C.Valid())
                {
                    if (C.Contains(P))
                    {
                        OutP = TileToMainscreenEx(T, 0, 0, -64,
                                                  TileHeights, TileSettings,
                                                  CameraX, CameraY, CameraZ,
                                                  ClientX, ClientY, ClientPlane,
                                                  CameraPitch, CameraYaw, ViewportScale,
                                                  ViewportWidth, ViewportHeight, ScreenType);
                        return T;
                    }
                }
            }

        return Tile();
    }

    bool SetZoom(double Zoom)
    {
        DEBUG_VERBOSE_START;

        if (!Mainscreen::WaitIsUpText(100, 25, "Walk here")) return false;

        std::int32_t ScrollAmount = NormalRandom(Profile::GetScrollAmount(), Profile::GetScrollAmount() * Profile::GetScrollAmountDeviation());
        const double CurrentZoom = Camera::GetZoom();
        constexpr double ScrollUpDifference = 0.03125; // Scroll up changes by 0.03125, down by 0.033854
        constexpr double ScrollDownDifference = 0.033854;
        ScrollDirection Direction;

        if (ScrollAmount < 1)
            ScrollAmount = 1;

        if (Zoom < CurrentZoom)
            Direction = ScrollDirection::SCROLL_DOWN;
        else
            Direction = ScrollDirection::SCROLL_UP;

        DEBUG_VERBOSE_LOG << "Direction: " << Direction << std::endl;

        if (((Direction == SCROLL_UP) ? (Zoom - CurrentZoom) : (CurrentZoom - Zoom)) < (((Direction == SCROLL_UP) ? ScrollUpDifference : ScrollDownDifference) * ScrollAmount)) // already within tolerance
        {
            DEBUG_VERBOSE_SUCCESS_NOINFO;
            return true;
        }

        Counter C = Counter(((Direction == SCROLL_UP) ? (Zoom - CurrentZoom) : (CurrentZoom - Zoom)) / ((Direction == SCROLL_UP) ? ScrollUpDifference : ScrollDownDifference));
        Counter S = Counter(ScrollAmount);
        while (C.Increment())
        {
            if (Terminate)
            {
                TerminateScript();
                DEBUG_VERBOSE_FAIL << "Terminating script" << std::endl;
                return false;
            }

            if (((Direction == SCROLL_UP) ? (Zoom - Camera::GetZoom()) : (Camera::GetZoom() - Zoom)) < (((Direction == SCROLL_UP) ? ScrollUpDifference : ScrollDownDifference) * ScrollAmount))
            {
                DEBUG_VERBOSE_SUCCESS_NOINFO;
                return true;
            }

            ((Direction == SCROLL_UP) ? ScrollUp() : ScrollDown());
            Wait(NormalRandom(Profile::GetScrollGapMean(), Profile::GetScrollGapMean() * Profile::GetScrollGapDeviation()));
            if (!S.Increment())
            {
                ScrollAmount = NormalRandom(Profile::GetScrollAmount(), Profile::GetScrollAmount() * Profile::GetScrollAmountDeviation());
                S = Counter(ScrollAmount);
                Wait(NormalRandom(Profile::GetScrollDelayMean(), Profile::GetScrollDelayMean() * Profile::GetScrollDelayDeviation()));
            }
        }

        if (((Direction == SCROLL_UP) ? (Zoom - Camera::GetZoom()) : (Camera::GetZoom() - Zoom)) < (((Direction == SCROLL_UP) ? ScrollUpDifference : ScrollDownDifference) * ScrollAmount))
        {
            DEBUG_VERBOSE_SUCCESS_NOINFO;
            return true;
        }

        DEBUG_VERBOSE_FAIL_EOF;
        return false;
    }
}