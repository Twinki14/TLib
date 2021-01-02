#include <Game/Tools/Settings.hpp>
#include <Core/Internal.hpp>
#include <Game/Models/Players.hpp>
#include <Game/Models/NPCs.hpp>
#include <Game/Models/GameObjects.hpp>
#include <Game/Models/DecorativeObjects.hpp>
#include <Game/Tools/Widgets.hpp>
#include <Game/Tools/Pathfinding.hpp>
#include <Game/Tools/Profile.hpp>
#include <Game/Tools/Interact.hpp>
#include <Game/Interfaces/Bank.hpp>
#include <Game/Interfaces/Minimap.hpp>
#include <Game/Interfaces/GameTabs/Options.hpp>
#include <Game/Interfaces/GameTabs/Combat.hpp>
#include <Game/Interfaces/GameTabs/Stats.hpp>
#include "../../Include/Utilities/Poh.hpp"
#include "../../Include/Utilities/Extensions.hpp"
#include "../../Include/Utilities/Mainscreen.hpp"
#include "../../Include/TScript.hpp"

bool POH_ClickObject(const Internal::Object& O, const std::string& Option, const std::function<bool()>& StateChangeFunc, bool AllowBoredom = false)
{
    if (!O) return false;

    const auto GameObj = (O.InstanceOf(Internal::GameObject::GetClass())) ? (Interactable::GameObject(Internal::GameObject(O))) : (Interactable::GameObject(nullptr));
    const auto DecObj = (O.InstanceOf(Internal::DecorativeObject::GetClass())) ? (Interactable::DecorativeObject(Internal::DecorativeObject(O))) : (Interactable::DecorativeObject(nullptr));
    if (!GameObj && !DecObj)
        return false;

    const auto Pos = Minimap::GetPosition();
    double Visibility = (GameObj) ? (GameObj.GetVisibility()) : (DecObj.GetVisibility());
    Tile ReachableTile = (GameObj) ? (GameObj.GetReachableTile()) : (DecObj.GetReachableTile());
    double Distance = ReachableTile.DistanceFrom(Pos);

    if (!ReachableTile)
        return false;

    bool InitClickObj = Visibility >= 1.00 && Distance <= 10;
    bool InitWalkPath_MS = false;
    bool InitWalkPath_MM = false;

    if (!InitClickObj)
    {
        InitWalkPath_MS = Distance > 10 && UniformRandom() <= 0.65;
        InitWalkPath_MM = !InitWalkPath_MS;
    }

    bool Walked = false;
    if (!InitClickObj)
    {
        auto Path = Pathfinding::FindPathTo(ReachableTile);
        if (Path.empty()) return false;

        auto WalkFunc = [&]() -> bool
        {
            if (Terminate) return true;

            Visibility = (GameObj) ? (GameObj.GetVisibility()) : (DecObj.GetVisibility());
            Path = Pathfinding::FindPathTo(ReachableTile);

            return (Path.size() <= 8 && Visibility == 1.00) || Path.size() <= 4;
        };

        if (InitWalkPath_MS)
        {
            while (!Walked)
            {
                if (Terminate) return false;

                bool Clicked = false;
                Tile TileClicked;

                for (auto I = Path.rbegin(); I != Path.rend(); I++)
                {
                    if (I->DistanceFrom(Minimap::GetPosition()) <= 12 && Mainscreen::IsTileVisible(*I))
                    {
                        if (Mainscreen::ClickTile(*I) && WaitFunc(1000, 10, Mainscreen::IsMoving, true))
                        {
                            Clicked = true;
                            TileClicked = *I;
                            break;
                        }
                    }
                }

                if (!Clicked) return false;

                Countdown Failsafe = Countdown(15000);
                while (Failsafe)
                {
                    if (!WaitFunc(1000, 10, Mainscreen::IsMoving, true)) break;
                    if (TileClicked && TileClicked.DistanceFrom(Minimap::GetPosition()) <= 2) break;
                }
                if (WalkFunc())
                    Walked = true;
            }

        } else if (InitWalkPath_MM)
        {
            Walked = Minimap::WalkPath(Path, 2, 1, WalkFunc);
        }
    }

    bool WaitUntilStopped = Mainscreen::IsMoving() && UniformRandom() <= 0.45;
    bool BoredomSpamClick = AllowBoredom && UniformRandom() <= 0.35;

    if (Walked || InitClickObj)
    {
        if (WaitUntilStopped)
        {
            WaitFunc(8250, 250, Mainscreen::IsMoving, false);
            Wait(NormalRandom(1250, 1250 * 0.10));
        }

        if (BoredomSpamClick && StateChangeFunc)
        {
            std::int32_t Amount = UniformRandom(2, 4);

            bool Interacted = false;
            const auto Var_RawInteractableMean = Profile::GetRawInteractableMean();
            const auto Var_RawMoveMean = Profile::GetRawInteractableMean();
            Profile::SetRawInteractableMean(25);
            Profile::SetRawMoveMean(75);

            for (std::uint32_t I = 0; I < Amount; I++)
            {
                Interacted = ((GameObj) ? (GameObj.Interact(Option)) : (DecObj.Interact(Option)));
                if (Interacted && StateChangeFunc())
                    break;
            }

            Profile::SetRawInteractableMean(Var_RawInteractableMean);
            Profile::SetRawMoveMean(Var_RawMoveMean);
            return Interacted;
        } else
            return (GameObj) ? (GameObj.Interact(Option)) : (DecObj.Interact(Option));
    }

    return false;
}

POH::POH_LOCATION POH::GetLocation()
{
    std::int32_t Setting = Settings::GetSetting(Globals::SETTING_POH_LOCATION.Index);
    std::uint32_t Bits = (Setting >> Globals::SETTING_POH_LOCATION.BitPos) & Globals::SETTING_POH_LOCATION.BitMask;
    return (POH_LOCATION) Bits;
}

bool POH::IsInside()
{
    const auto MapRegions = Internal::GetMapRegions();
    return !MapRegions.empty() && MapRegions[0] == Globals::MAP_REGION_POH;
}

bool POH::InBuildMode()
{
    if (POH::IsInside())
    {
        auto Varbit = Internal::GetVarbit(2176);
        int Setting = Settings::GetSetting(Varbit.GetSettingID());
        int mask = (1 << ((Varbit.GetMSB() - Varbit.GetLSB()) + 1)) - 1;
        return Setting > 0 || Settings::GetSetting(Globals::SETTING_POH_BUILD_MODE.Index) > 0;
    }
    return false;
}

bool POH::IsLoading()
{
    return POH::IsInside() && Settings::GetSetting(Globals::SETTING_POH_LOADING.Index) > 512;
}

bool POH::IsHouseMenuOpen()
{
    return POH::GetHouseMenuCloseWidget().IsVisible();
}

bool POH::Enter(bool BuildMode)
{
    if ((POH::IsInside() || POH::IsLoading()) && ((BuildMode) ? (POH::InBuildMode()) : (true))) return true;

    auto FilterPortal = [](const Interactable::GameObject& G) -> bool
    {
        if (G)
        {
            const auto Info = G.GetInfo();
            if (Info)
            {
                const auto Name = Info.GetName();
                if (Name == "Portal")
                {
                    const auto Actions = Info.GetActions();
                    for (const auto& A : Actions)
                        if (A == "Home")
                            return true;
                }
            }
        }
        return false;
    };

    auto Inside_Loading = [&BuildMode]() -> bool
    {
        if (Terminate) return false;

        if (Bank::IsEnterPinOpen() && Bank::EnterPin())
            return false;

        return POH::IsInside() || POH::IsLoading();
    };

    auto Inside = [&BuildMode]() -> bool { return POH::IsInside() && ((BuildMode) ? POH::InBuildMode() : true); };
    const auto Portal = GameObjects::Get(FilterPortal);
    if (Portal && POH_ClickObject(Portal, (BuildMode) ? ("Build") : ("Home"), Inside_Loading, false))
    {
        if (WaitFunc(8250, 250, Inside_Loading, true))
            return WaitFunc(12500, 250, Inside, true);
    }
    return POH::IsInside();
}

bool POH::Exit()
{
    if (!POH::IsInside()) return true;

    auto FilterPortal = [](const Interactable::GameObject& G) -> bool
    {
        if (G)
        {
            const auto Info = G.GetInfo();
            if (Info)
            {
                const auto Name = Info.GetName();
                if (Name == "Portal")
                {
                    const auto Actions = Info.GetActions();
                    for (const auto& A : Actions)
                        if (A == "Lock")
                            return true;
                }
            }
        }
        return false;
    };

    const auto Portal = GameObjects::Get(FilterPortal);
    auto Outside = []() -> bool { return !POH::IsInside(); };
    if (Portal && POH_ClickObject(Portal, "Enter", Outside))
        return WaitFunc(8250, 250, POH::IsInside, false);
    return false;
}

bool POH::CallServant()
{
    if (!POH::IsInside()) return false;

    const auto NPC = POH::GetServant();
    const auto Player = Players::GetLocal();
    if (!NPC || NPC.GetInteracting() != Player)
    {
        if (!POH::OpenHouseMenu())
            return false;

        auto ButtonWidget = POH::GetHouseMenuServantWidget();
        return ButtonWidget.Interact() && WaitFunc(2250, 250, [&Player]() -> bool { return POH::GetServant().GetInteracting() == Player; });
    }
    return true;
}

bool POH::ToggleBuildMode(bool Toggle)
{
    if (!POH::IsInside()) return false;
    if (POH::InBuildMode() == Toggle) return true;
    if (POH::OpenHouseMenu())
    {
        auto ToggleWidget = POH::GetHouseMenuBuildToggleWidget();
        if (ToggleWidget.Interact() && WaitFunc(1750, 125, POH::IsLoading, true))
            return WaitFunc(3650, 425, [&Toggle]() -> bool { return !POH::IsLoading() && POH::InBuildMode() == Toggle; });
    }
    return POH::InBuildMode() == Toggle;
}

bool POH::OpenHouseMenu()
{
    auto Button = POH::GetHouseButtonWidget();
    auto ServantButton = POH::GetHouseMenuServantWidget();

    if (ServantButton.IsVisible()) return true;
    if (!Button.IsVisible() && !Options::Open()) return false;

    Button = POH::GetHouseButtonWidget();
    return Button.Interact() && WaitFunc(1275, 225, POH::IsHouseMenuOpen, true);
}

bool POH::CloseHouseMenu()
{
    auto CloseButton = POH::GetHouseMenuCloseWidget();
    if (!CloseButton.IsVisible()) return false;

    return CloseButton.Interact() && WaitFunc(1275, 225, POH::IsHouseMenuOpen, false);
}

Interactable::Widget POH::GetHouseButtonWidget()
{
    /*constexpr std::uint32_t Parent = 261;
    constexpr std::uint32_t Width = 190;
    constexpr std::uint32_t Height = 40;
    constexpr std::uint32_t RelativeY = 220;
    constexpr std::uint32_t Child_Width = 40;
    constexpr std::uint32_t Child_Height = 40;
    constexpr std::uint32_t Child_RelativeX = 98;
    constexpr std::uint32_t Child_SpriteID = 761;

    const auto ParentWidget = Widgets::Get(Parent);
    const auto Children = ParentWidget.GetChildren();
    DebugLog(Children.size());
    for (const auto& C : Children)
    {
        DebugLog(C.GetText());
        DebugLog(C.GetWidth());
        DebugLog(C.GetHeight());
        DebugLog(C.GetRelativeY());

        if (!C.GetText().empty()) continue;
        if (C.GetWidth() != Width) continue;
        if (C.GetHeight() != Height) continue;
        if (C.GetRelativeY() != RelativeY) continue;

        for (const auto& GC : C.GetChildren())
        {
            DebugLog(GC.GetText());
            DebugLog(GC.GetWidth());
            DebugLog(GC.GetHeight());
            DebugLog(GC.GetRelativeX());
            DebugLog(GC.GetSpriteID());

            if (!GC.GetText().empty()) continue;
            if (GC.GetWidth() != Child_Width) continue;
            if (GC.GetHeight() != Child_Height) continue;
            if (GC.GetRelativeX() != Child_RelativeX) continue;
            if (GC.GetSpriteID() != Child_SpriteID) continue;
            return GC;
        }
    }*/

    return Widgets::Get(261, 100);
}

Interactable::Widget POH::GetHouseMenuCloseWidget()
{
/*    constexpr std::uint32_t Parent = 370;
    constexpr std::uint32_t Width = 26;
    constexpr std::uint32_t Height = 23;
    constexpr std::uint32_t SpriteID = 535;

    const auto ParentWidget = Widgets::Get(Parent);
    const auto Children = ParentWidget.GetChildren();
    for (const auto& C : Children)
    {
        if (!C.GetText().empty()) continue;
        if (C.GetWidth() != Width) continue;
        if (C.GetHeight() != Height) continue;
        if (C.GetSpriteID() != SpriteID) continue;
        return C;
    }*/
    return Widgets::Get(370, 21);
}

Interactable::Widget POH::GetHouseMenuServantWidget()
{
/*    constexpr std::uint32_t Parent = 370;
    constexpr std::uint32_t Width = 170;
    constexpr std::uint32_t Height = 36;
    constexpr std::uint32_t RelativeX = 10;
    constexpr std::uint32_t RelativeY = 205;

    const auto ParentWidget = Widgets::Get(Parent);
    const auto Children = ParentWidget.GetChildren();
    for (const auto& C : Children)
    {
        if (!C.GetText().empty()) continue;
        if (C.GetWidth() != Width) continue;
        if (C.GetHeight() != Height) continue;
        if (C.GetRelativeX() != RelativeX) continue;
        if (C.GetRelativeY() != RelativeY) continue;
        return C;
    }*/
    return Widgets::Get(370, 19);
}

Interactable::Widget POH::GetHouseMenuBuildToggleWidget()
{
/*
    constexpr std::uint32_t Parent = 370;
    constexpr std::uint32_t Width = 17;
    constexpr std::uint32_t Height = 17;
    constexpr std::uint32_t RelativeY = 68;
    constexpr std::uint32_t SpriteID = 697; // Sprite that isn't 'ticked'

    const auto ParentWidget = Widgets::Get(Parent);
    const auto Children = ParentWidget.GetChildren();
    for (const auto& C : Children)
    {
        if (!C.GetText().empty()) continue;

        if (C.GetWidth() != Width) continue;
        if (C.GetHeight() != Height) continue;

        if (C.GetRelativeY() != RelativeY) continue;

        if (C.GetSpriteID() != SpriteID) continue;

        return C;
    }
*/

    return Interactable::Widget();
}

Interactable::Widget POH::GetHouseMenuTeleportToggleWidget()
{
/*
    constexpr std::uint32_t Parent = 370;
    constexpr std::uint32_t Width = 17;
    constexpr std::uint32_t Height = 17;
    constexpr std::uint32_t RelativeY = 88;
    constexpr std::uint32_t SpriteID = 697; // Sprite that isn't 'ticked'

    const auto ParentWidget = Widgets::Get(Parent);
    const auto Children = ParentWidget.GetChildren();
    for (const auto& C : Children)
    {
        if (!C.GetText().empty()) continue;

        if (C.GetWidth() != Width) continue;
        if (C.GetHeight() != Height) continue;

        if (C.GetRelativeY() != RelativeY) continue;

        if (C.GetSpriteID() != SpriteID) continue;

        return C;
    }
*/

    return Interactable::Widget();
}

Interactable::NPC POH::GetServant()
{
    if (!POH::IsInside()) return Interactable::NPC(nullptr);

    static std::vector<std::int32_t> IDs;
    if (IDs.empty())
        for (const auto& [ID, Def] : Globals::POH_SERVANT_DEFS)
            IDs.emplace_back(ID);

    return NPCs::Get(IDs);
}

Internal::Object POH::Get(const POH::PORTAL_NEXUS_ROOM& Obj)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    switch (Obj)
    {
        case MOUNTED_DIGSITE_PENDANT: return DecorativeObjects::Get(Globals::OBJECTS_PORTAL_NEXUS_ROOM[Obj]);
        case MOUNTED_XERICS_TALISMAN: return DecorativeObjects::Get(Globals::OBJECTS_PORTAL_NEXUS_ROOM[Obj]);
        case PORTAL_NEXUS: return GameObjects::Get(Globals::OBJECTS_PORTAL_NEXUS_ROOM[Obj]);
        default: return Internal::Object(nullptr);
    }
}

Internal::Object POH::Get(const POH::SUPERIOR_GARDEN_ROOM& Obj)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    switch (Obj)
    {
        case RESTORATION_POOL:          return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case REVITALISATION_POOL:       return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case REJUVENATION_POOL:         return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case FANCY_REJUVENATION_POOL:   return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case ORNATE_REJUVENATION_POOL:  return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case SPIRIT_TREE:               return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case OBELISK:                   return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case FAIRY_RING:                return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        case SPIRIT_TREE_FAIRY_RING:    return GameObjects::Get(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);
        default: return Internal::Object(nullptr);
    }
}

Internal::Object POH::Get(const POH::ACHIEVEMENT_GALLERY_ROOM& Obj)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    switch (Obj)
    {
        case BASIC_JEWELLERY_BOX:  return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        case FANCY_JEWELLERY_BOX:  return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        case ORNATE_JEWELLERY_BOX: return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        case ANCIENT:   return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        case LUNAR:     return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        case DARK:      return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        case OCCULT:    return GameObjects::Get(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);
        default: return Internal::Object(nullptr);
    }
}

Internal::Object POH::Get(const POH::WORKSHOP_ROOM& Obj)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    switch (Obj)
    {
        case ARMOUR_STAND: return GameObjects::Get(Globals::OBJECTS_WORKSHOP_ROOM[Obj]);
        default: return Internal::Object(nullptr);
    }
}

Internal::Object POH::Get(const std::vector<PORTAL_NEXUS_ROOM>& Objs)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    std::vector<std::string> Names;
    for (const auto& Obj : Objs)
        if (Obj >= 0 && Obj < Globals::OBJECTS_PORTAL_NEXUS_ROOM.size())
            Names.emplace_back(Globals::OBJECTS_PORTAL_NEXUS_ROOM[Obj]);

    if (Names.empty())
        return Internal::Object(nullptr);

    if (Objs[0] == PORTAL_NEXUS)
        return GameObjects::Get(Names);

    return DecorativeObjects::Get(Names);
}

Internal::Object POH::Get(const std::vector<SUPERIOR_GARDEN_ROOM>& Objs)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    std::vector<std::string> Names;
    for (const auto& Obj : Objs)
        if (Obj >= 0 && Obj < Globals::OBJECTS_SUPERIOR_GARDEN_ROOM.size())
            Names.emplace_back(Globals::OBJECTS_SUPERIOR_GARDEN_ROOM[Obj]);

    if (Names.empty())
        return Internal::Object(nullptr);
    return GameObjects::Get(Names);
}

Internal::Object POH::Get(const std::vector<ACHIEVEMENT_GALLERY_ROOM>& Objs)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    std::vector<std::string> Names;
    for (const auto& Obj : Objs)
        if (Obj >= 0 && Obj < Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM.size())
            Names.emplace_back(Globals::OBJECTS_ACHIEVEMENT_GALLERY_ROOM[Obj]);

    if (Names.empty())
        return Internal::Object(nullptr);
    return GameObjects::Get(Names);
}

Internal::Object POH::Get(const std::vector<WORKSHOP_ROOM>& Objs)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    std::vector<std::string> Names;
    for (const auto& Obj : Objs)
        if (Obj >= 0 && Obj < Globals::OBJECTS_WORKSHOP_ROOM.size())
            Names.emplace_back(Globals::OBJECTS_WORKSHOP_ROOM[Obj]);

    if (Names.empty())
        return Internal::Object(nullptr);
    return GameObjects::Get(Names);
}

bool POH::Has(const POH::PORTAL_NEXUS_ROOM& Obj)
{
    return POH::Get(Obj);
}

bool POH::Has(const POH::SUPERIOR_GARDEN_ROOM& Obj)
{
    return POH::Get(Obj);
}

bool POH::Has(const POH::ACHIEVEMENT_GALLERY_ROOM& Obj)
{
    return POH::Get(Obj);
}

bool POH::Has(const POH::WORKSHOP_ROOM& Obj)
{
    return POH::Get(Obj);
}

bool POH::Has(const POH::TELEPORT& Teleport)
{
    switch (Teleport)
    {
        case NEXUS_VARROCK...NEXUS_TROLL_STRONGHOLD:
        {
            const auto[CloseWidget, Text, InteractableWidget] = POH::FindTeleportWidget(Teleport);
            return CloseWidget.IsVisible() && InteractableWidget.IsVisible();
        }
        default: return POH::GetTeleportObject(Teleport);
    }
}

bool POH::Teleport(const POH::TELEPORT& Teleport, bool UseKeyboard)
{
    if (!POH::IsInside()) return false;
    if (POH::IsLoading()) return false;

    auto Outside = []() -> bool { return !POH::IsInside() && Mainscreen::GetState() != Mainscreen::LOADING; };

    Internal::Object RawObj = POH::GetTeleportObject(Teleport);
    if (!RawObj) return false;

    const auto& TeleportAction = Globals::POH_TELEPORT_ACTIONS.at(Teleport);
    auto [CloseWidget, Text, InteractWidget] = POH::FindTeleportWidget(Teleport);
    auto KeyShortcut = POH::FindTeleportWidgetKeyShortcut(Text, Teleport);

    switch (Teleport)
    {
        case NEXUS_VARROCK...NEXUS_TROLL_STRONGHOLD:
        {
            if (!CloseWidget.IsVisible())
            {
                const auto Obj = Interactable::GameObject(Internal::GameObject(RawObj));
                if (!Obj) return false;
                const auto Info = Obj.GetInfo();
                if (!Info) return false;

                const auto Actions = Info.GetActions();

                bool CanLeftClick = false;
                for (const auto& A : Actions)
                {
                    //std::cout << A << " | " << TeleportAction << std::endl;
                    if (A.find(TeleportAction) != std::string::npos)
                    {
                        CanLeftClick = true;
                        break;
                    }
                }

                if (CanLeftClick)
                    if (POH_ClickObject(RawObj, TeleportAction, Outside))
                        return WaitFunc(8250, 250, Outside, true);

                auto WidgetVisible = [&]() -> bool
                {
                    auto [CloseWidget, Text, InteractWidget] = POH::FindTeleportWidget(Teleport);
                    return CloseWidget.IsVisible();
                };

                if (!POH_ClickObject(RawObj, "Teleport Menu", WidgetVisible, false))
                    return false;

                if (!WaitFunc(8250, 250, WidgetVisible, true))
                    return false;

                std::tie(CloseWidget, Text, InteractWidget) = POH::FindTeleportWidget(Teleport);
                KeyShortcut = POH::FindTeleportWidgetKeyShortcut(Text, Teleport);
            }

            if (CloseWidget.IsVisible())
            {
                if (!InteractWidget.IsVisible()) return false; // Interface open, interact widget not found (doesn't have)

                if (UseKeyboard && KeyShortcut != '\0')
                    return Interact::TypeKey((int) KeyShortcut) && WaitFunc(8250, 250, Outside, true);

                const auto VisibleBoundsWidget = Widgets::Get(Globals::WIDGET_PORTAL_NEXUS_TELEPORTS_CONTAINER_BOUNDS.Parent, Globals::WIDGET_PORTAL_NEXUS_TELEPORTS_CONTAINER_BOUNDS.Child);
                if (!VisibleBoundsWidget) return false;

                auto InteractWidgetBox = InteractWidget.GetBox();
                const auto VisibleBox = VisibleBoundsWidget.GetBox();

                Countdown Failsafe(10000);
                while (!Failsafe.IsFinished())
                {
                    if (Terminate) TerminateScript();

                    if (VisibleBox.Contains(InteractWidgetBox))
                        break;

                    if (!VisibleBox.Contains(GetMousePos()))
                        Interact::MoveMouse(VisibleBox);

                    auto Direction = (ScrollDirection) -1;

                    InteractWidgetBox = InteractWidget.GetBox();
                    if (InteractWidgetBox.GetY2() > VisibleBox.GetY2())
                        Direction = SCROLL_DOWN;
                    if (InteractWidgetBox.Y < VisibleBox.Y)
                        Direction = SCROLL_UP;

                    std::cout << Direction << std::endl;
                    Interact::Scroll(Direction);
                    InteractWidgetBox = InteractWidget.GetBox();
                }

                if (VisibleBox.Contains(InteractWidgetBox))
                    return InteractWidget.Interact("Continue") && WaitFunc(8250, 250, Outside, true);
            }

            return false;
        }

        case WALL_DIGSITE...WALL_LITHKREN: // - Digsite pendant
        {
            if (!CloseWidget.IsVisible())
            {
                const auto Obj = Interactable::DecorativeObject(Internal::DecorativeObject(RawObj));
                if (!Obj) return false;
                const auto Info = Obj.GetInfo();
                if (!Info) return false;

                const auto Actions = Info.GetActions();
                bool CanLeftClick = false;
                for (const auto& A : Actions)
                {
                    if (A.find(TeleportAction) != std::string::npos)
                    {
                        CanLeftClick = true;
                        break;
                    }
                }

                if (CanLeftClick)
                    if (POH_ClickObject(RawObj, TeleportAction, Outside))
                        return WaitFunc(8250, 250, Outside, true);

                auto WidgetVisible = [&]() -> bool
                {
                    auto [CloseWidget, Text, InteractWidget] = POH::FindTeleportWidget(Teleport);
                    return CloseWidget.IsVisible();
                };

                if (!POH_ClickObject(RawObj, "Teleport menu", WidgetVisible, false))
                    return false;

                if (!WaitFunc(8250, 250, WidgetVisible, true))
                    return false;

                std::tie(CloseWidget, Text, InteractWidget) = POH::FindTeleportWidget(Teleport);
                KeyShortcut = POH::FindTeleportWidgetKeyShortcut(Text, Teleport);
            }

            if (CloseWidget.IsVisible())
            {
                if (!InteractWidget.IsVisible()) return false; // Interface open, interact widget not found (doesn't have)

                if (UseKeyboard && KeyShortcut != '\0')
                    return Interact::TypeKey((int) KeyShortcut) && WaitFunc(8250, 250, Outside, true);
                return InteractWidget.Interact("Continue") && WaitFunc(8250, 250, Outside, true);
            }

            return false;
        }

        case WALL_XERICS_LOOKOUT...WALL_XERICS_HONOUR: // - Xeric's Talisman
        {
            if (!CloseWidget.IsVisible())
            {
                const auto Obj = Interactable::DecorativeObject(Internal::DecorativeObject(RawObj));
                if (!Obj) return false;

                bool CanLeftClick = false;
                static const auto XericsStr = std::string("Xeric's ");
                const auto StrPos = TeleportAction.find(XericsStr);
                std::string LeftClickAction;
                if (StrPos == 0)
                {
                    LeftClickAction = TeleportAction.substr(XericsStr.length());
                    std::replace( LeftClickAction.begin(), LeftClickAction.end(), '-', ' ');
                    const auto Info = Obj.GetInfo();
                    if (Info)
                    {
                        const auto Actions = Info.GetActions();
                        for (const auto& A : Actions)
                        {
                            if (A.find(LeftClickAction) != std::string::npos)
                            {
                                CanLeftClick = true;
                                break;
                            }
                        }
                    }
                }

                if (CanLeftClick)
                    if (POH_ClickObject(RawObj, LeftClickAction, Outside))
                        return WaitFunc(8250, 250, Outside, true);

                auto WidgetVisible = [&]() -> bool
                {
                    auto [CloseWidget, Text, InteractWidget] = POH::FindTeleportWidget(Teleport);
                    return CloseWidget.IsVisible();
                };

                if (!POH_ClickObject(RawObj, "Teleport menu", WidgetVisible, false))
                    return false;

                if (!WaitFunc(8250, 250, WidgetVisible, true))
                    return false;

                std::tie(CloseWidget, Text, InteractWidget) = POH::FindTeleportWidget(Teleport);
                KeyShortcut = POH::FindTeleportWidgetKeyShortcut(Text, Teleport);
            }

            if (CloseWidget.IsVisible())
            {
                if (!InteractWidget.IsVisible()) return false; // Interface open, interact widget not found (doesn't have)

                if (UseKeyboard && KeyShortcut != '\0')
                    return Interact::TypeKey((int) KeyShortcut) && WaitFunc(8250, 250, Outside, true);
                return InteractWidget.Interact("Continue") && WaitFunc(8250, 250, Outside, true);
            }

            return false;
        }

        case JEWELLERY_DUAL_ARENA...JEWELLERY_AL_KHARID:
        {
            if (!CloseWidget.IsVisible())
            {
                const auto Obj = Interactable::GameObject(Internal::GameObject(RawObj));
                if (!Obj) return false;

                auto WidgetVisible = [&]() -> bool
                {
                    auto [CloseWidget, Text, InteractWidget] = POH::FindTeleportWidget(Teleport);
                    return CloseWidget.IsVisible();
                };

                if (!POH_ClickObject(RawObj, "Teleport", WidgetVisible, false))
                    return false;

                if (!WaitFunc(8250, 250, WidgetVisible, true))
                    return false;

                std::tie(CloseWidget, Text, InteractWidget) = POH::FindTeleportWidget(Teleport);
                KeyShortcut = POH::FindTeleportWidgetKeyShortcut(Text, Teleport);
            }

            if (CloseWidget.IsVisible())
            {
                if (!InteractWidget.IsVisible()) return false; // Interface open, interact widget not found (doesn't have)

                if (Text.find("<str>") != std::string::npos) // Not unlocked
                    return false;

                if (UseKeyboard && KeyShortcut != '\0')
                    return Interact::TypeKey((int) KeyShortcut) && WaitFunc(8250, 250, Outside, true);
                return InteractWidget.Interact(TeleportAction) && WaitFunc(8250, 250, Outside, true);
            }
        }

        default: return false;
    }
    return false;
}

bool POH::Drink(const POH::SUPERIOR_GARDEN_ROOM& Pool, bool Force)
{
    if (!POH::IsInside()) return false;
    if (POH::IsLoading()) return false;

    auto HasSpecialAttack = []() -> bool { return Combat::GetSpecialAttack() >= 100; };
    auto HasRunEnergy = []() -> bool { return Stats::GetRunEnergy() >= 95; };
    auto HasPrayerPoints = []() -> bool { return Stats::GetCurrentLevel(Stats::PRAYER) == Stats::GetRealLevel(Stats::PRAYER); };
    auto HasReducedStats = [](bool IncludeHitpoints) -> bool
    {
        const auto CurrentLevels = Internal::GetCurrentLevels();
        const auto RealLevels = Internal::GetLevels();

        for (std::int32_t I = Stats::ATTACK; I != Stats::ALL; I++)
        {
            if (I == Stats::HITPOINTS && !IncludeHitpoints)
                continue;

            if (CurrentLevels[I] < RealLevels[I])
                return true;
        }

        return false;
    };

    if (!Force)
    {
        switch (Pool)
        {
            case RESTORATION_POOL: if (HasSpecialAttack()) return true; break;
            case REVITALISATION_POOL: if (HasSpecialAttack() && HasRunEnergy()) return true; break;
            case REJUVENATION_POOL: if (HasSpecialAttack() && HasRunEnergy() && HasPrayerPoints()) return true; break;
            case FANCY_REJUVENATION_POOL: if (HasSpecialAttack() && HasRunEnergy() && HasPrayerPoints() && !HasReducedStats(false)) return true; break;
            case ORNATE_REJUVENATION_POOL: if (HasSpecialAttack() && HasRunEnergy() && HasPrayerPoints() && !HasReducedStats(true)) return true; break;
            default: return false;
        }
    }

    switch (Pool)
    {
        case RESTORATION_POOL...ORNATE_REJUVENATION_POOL:
        {
            const auto Obj = POH::Get(Pool);
            if (!Obj) return false;

            auto Drank = [&]() -> bool
            {
                if (Mainscreen::IsMoving()) return false;

                switch (Pool)
                {
                    case RESTORATION_POOL: if (HasSpecialAttack()) return true; break;
                    case REVITALISATION_POOL: if (HasSpecialAttack() && HasRunEnergy()) return true; break;
                    case REJUVENATION_POOL: if (HasSpecialAttack() && HasRunEnergy() && HasPrayerPoints()) return true; break;
                    case FANCY_REJUVENATION_POOL: if (HasSpecialAttack() && HasRunEnergy() && HasPrayerPoints() && !HasReducedStats(false)) return true; break;
                    case ORNATE_REJUVENATION_POOL: if (HasSpecialAttack() && HasRunEnergy() && HasPrayerPoints() && !HasReducedStats(true)) return true; break;
                    default: return false;
                }

                return false;
            };

            if (POH_ClickObject(Obj, "Drink", Drank, true))
                return WaitFunc(8250, 250, Drank, true);
        }
        default: return false;
    }
}

std::tuple<Interactable::Widget, std::string, Interactable::Widget> POH::FindTeleportWidget(const POH::TELEPORT& Teleport)
{
    if (!POH::IsInside())
        return std::make_tuple(Interactable::Widget(nullptr), "", Interactable::Widget(nullptr));

    switch (Teleport)
    {
        case NEXUS_VARROCK...NEXUS_TROLL_STRONGHOLD:
        {
            auto CloseWidget = Widgets::Get(Globals::WIDGET_PORTAL_NEXUS_CLOSE.Parent, Globals::WIDGET_PORTAL_NEXUS_CLOSE.Child, Globals::WIDGET_PORTAL_NEXUS_CLOSE.Grandchild);

            if (!CloseWidget.IsVisible())
                return std::make_tuple(Interactable::Widget(nullptr), "", Interactable::Widget(nullptr));

            const auto InteractablesParent = Widgets::Get(Globals::WIDGET_PORTAL_NEXUS_TELEPORTS_INTERACTABLE_WIDGET_PARENT.Parent, Globals::WIDGET_PORTAL_NEXUS_TELEPORTS_INTERACTABLE_WIDGET_PARENT.Child);
            const auto TextsParent = Widgets::Get(Globals::WIDGET_PORTAL_NEXUS_TELEPORTS_TEXTS_PARENT.Parent, Globals::WIDGET_PORTAL_NEXUS_TELEPORTS_TEXTS_PARENT.Child);

            if (!InteractablesParent || !TextsParent)
                return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));

            auto InteractableChildren = InteractablesParent.GetChildren();
            auto TextsChildren = TextsParent.GetChildren();

            if (InteractableChildren.size() != TextsChildren.size())
                return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));

            const auto& Action = Globals::POH_TELEPORT_ACTIONS.at(Teleport);
            Interactable::Widget FoundTextWidget;
            Interactable::Widget FoundInteractableWidget;

            for (std::uint32_t I = 0; I < TextsChildren.size(); I++)
            {
                const auto Text = TextsChildren[I].GetText();
                if (Text.find(Action) != std::string::npos)
                {
                    FoundTextWidget = TextsChildren[I];
                    FoundInteractableWidget = InteractableChildren[I];
                    break;
                }
            }

            if (!FoundTextWidget.IsVisible() || !FoundInteractableWidget.IsVisible())
                return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));

            return std::make_tuple(std::move(CloseWidget), FoundTextWidget.GetText(), std::move(FoundInteractableWidget));
        }

        case WALL_DIGSITE...WALL_XERICS_HONOUR:
        {
            auto CloseWidget = Widgets::Get(Globals::WIDGET_PARCHMENT_TELEPORT_CLOSE.Parent, Globals::WIDGET_PARCHMENT_TELEPORT_CLOSE.Child);

            if (!CloseWidget.IsVisible())
                return std::make_tuple(Interactable::Widget(nullptr), "", Interactable::Widget(nullptr));

            const auto Parent = Widgets::Get(Globals::WIDGET_PARCHMENT_TELEPORT_OPTIONS.Parent, Globals::WIDGET_PARCHMENT_TELEPORT_OPTIONS.Child);
            const auto Children = Parent.GetChildren();
            if (!Parent.IsVisible() || Children.empty())
                return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));

            const auto& Action = Globals::POH_TELEPORT_ACTIONS.at(Teleport);
            Interactable::Widget FoundWidget;

            for (const auto& W : Children)
            {
                const auto Text = W.GetText();
                if (Text.find(Action) != std::string::npos)
                {
                    FoundWidget = W;
                    break;
                }
            }

            if (FoundWidget.IsVisible())
                return std::make_tuple(std::move(CloseWidget), FoundWidget.GetText(), std::move(FoundWidget));

            return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));
        }

        case JEWELLERY_DUAL_ARENA...JEWELLERY_AL_KHARID:
        {
            auto CloseWidget = Widgets::Get(Globals::WIDGET_JEWELLERY_BOX_CLOSE.Parent, Globals::WIDGET_JEWELLERY_BOX_CLOSE.Child);

            if (!CloseWidget.IsVisible())
                return std::make_tuple(Interactable::Widget(nullptr), "", Interactable::Widget(nullptr));

            Interactable::Widget Parent;
            switch (Teleport)
            {
                case JEWELLERY_DUAL_ARENA...JEWELLERY_CLAN_WARS:        Parent = Widgets::Get(Globals::WIDGET_JEWELLERY_BOX_RING_OF_DUELING.Parent, Globals::WIDGET_JEWELLERY_BOX_RING_OF_DUELING.Child); break;
                case JEWELLERY_BURTHORPE...JEWELLERY_WINTERTODT_CAMP:   Parent = Widgets::Get(Globals::WIDGET_JEWELLERY_BOX_GAMES_NECKLACE.Parent, Globals::WIDGET_JEWELLERY_BOX_GAMES_NECKLACE.Child); break;
                case JEWELLERY_WARRIORS_GUILD...JEWELLERY_RANGING_GUILD:Parent = Widgets::Get(Globals::WIDGET_JEWELLERY_BOX_COMBAT_BRACELET.Parent, Globals::WIDGET_JEWELLERY_BOX_COMBAT_BRACELET.Child); break;
                case JEWELLERY_FISHING_GUILD...JEWELLERY_FARMING_GUILD: Parent = Widgets::Get(Globals::WIDGET_JEWELLERY_BOX_SKILLS_NECKLACE.Parent, Globals::WIDGET_JEWELLERY_BOX_SKILLS_NECKLACE.Child); break;
                case JEWELLERY_MISCELLANIA...JEWELLERY_DONDAKANS_ROCK:  Parent = Widgets::Get(Globals::WIDGET_JEWELLERY_BOX_RING_OF_WEALTH.Parent, Globals::WIDGET_JEWELLERY_BOX_RING_OF_WEALTH.Child); break;
                case JEWELLERY_EDGEVILLE...JEWELLERY_AL_KHARID:         Parent = Widgets::Get(Globals::WIDGET_JEWELLERY_AMULET_OF_GLORY.Parent, Globals::WIDGET_JEWELLERY_AMULET_OF_GLORY.Child); break;
                default: break;
            }

            const auto Children = Parent.GetChildren();
            if (!Parent.IsVisible() || Children.empty())
                return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));

            const auto& Action = Globals::POH_TELEPORT_ACTIONS.at(Teleport);
            Interactable::Widget FoundWidget;

            for (const auto& W : Children)
            {
                const auto Text = W.GetText();
                if (Text.find(Action) != std::string::npos)
                {
                    FoundWidget = W;
                    break;
                }
            }

            if (FoundWidget.IsVisible())
                return std::make_tuple(std::move(CloseWidget), FoundWidget.GetText(), std::move(FoundWidget));

            return std::make_tuple(std::move(CloseWidget), "", Interactable::Widget(nullptr));
        }

        default: return std::make_tuple(Interactable::Widget(nullptr), "", Interactable::Widget(nullptr));
    }
}

char POH::FindTeleportWidgetKeyShortcut(const std::string& Text, const POH::TELEPORT& Teleport)
{
    if (Text.empty())
        return '\0';

    switch (Teleport)
    {
        case NEXUS_VARROCK...NEXUS_TROLL_STRONGHOLD: // <col=ffffff>1</col> :  Varrock
        case WALL_DIGSITE...WALL_XERICS_HONOUR: // <col=735a28>1</col>: Digsite
        {
            const auto MatchPos = Text.find("</col>");
            if (MatchPos != std::string::npos)
                return Text[MatchPos - 1];
            return '\0';
        }

        case JEWELLERY_DUAL_ARENA...JEWELLERY_AL_KHARID: // <col=ccccff>1:</col> Duel Arena
        {
            const auto MatchPos = Text.find(":</col>");
            if (MatchPos != std::string::npos)
                return Text[MatchPos - 1];
            return '\0';
        }

        default: return '\0';
    }
}

Internal::Object POH::GetTeleportObject(const POH::TELEPORT& Teleport)
{
    if (!POH::IsInside())
        return Internal::Object(nullptr);

    switch (Teleport)
    {
        case NEXUS_VARROCK...NEXUS_TROLL_STRONGHOLD: return POH::Get(POH::PORTAL_NEXUS);
        case WALL_DIGSITE...WALL_LITHKREN: return POH::Get(POH::MOUNTED_DIGSITE_PENDANT); // - Digsite pendant
        case WALL_XERICS_LOOKOUT...WALL_XERICS_HONOUR: return POH::Get(POH::MOUNTED_XERICS_TALISMAN); // - Xeric's Talisman
        case JEWELLERY_DUAL_ARENA...JEWELLERY_WINTERTODT_CAMP: return POH::Get(std::vector<POH::ACHIEVEMENT_GALLERY_ROOM> { POH::ORNATE_JEWELLERY_BOX, POH::FANCY_JEWELLERY_BOX, POH::BASIC_JEWELLERY_BOX }); // BASIC_JEWELLERY_BOX
        case JEWELLERY_WARRIORS_GUILD...JEWELLERY_FARMING_GUILD: return POH::Get(std::vector<POH::ACHIEVEMENT_GALLERY_ROOM> { POH::FANCY_JEWELLERY_BOX, POH::BASIC_JEWELLERY_BOX }); // FANCY_JEWELLERY_BOX
        case JEWELLERY_MISCELLANIA...JEWELLERY_AL_KHARID: return POH::Get(POH::ORNATE_JEWELLERY_BOX); // ORNATE_JEWELLERY_BOX
        default: return Internal::Object(nullptr);
    }
}

/**
case NEXUS_VARROCK...NEXUS_TROLL_STRONGHOLD:
{

}

case WALL_DIGSITE...WALL_LITHKREN: // - Digsite pendant
{

}

case WALL_XERICS_LOOKOUT...WALL_XERICS_HONOUR: // - Xeric's Talisman
{

}


case JEWELLERY_DUAL_ARENA...JEWELLERY_WINTERTODT_CAMP: // BASIC_JEWELLERY_BOX
{

}

case JEWELLERY_WARRIORS_GUILD...JEWELLERY_FARMING_GUILD: // FANCY_JEWELLERY_BOX
{

}

case JEWELLERY_MISCELLANIA...JEWELLERY_AL_KHARID: // ORNATE_JEWELLERY_BOX
{

}
 **/