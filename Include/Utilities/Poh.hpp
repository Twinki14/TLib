#ifndef TLIB_POH_HPP_INCLUDED
#define TLIB_POH_HPP_INCLUDED

#include <Core/Globals.hpp>
#include <Game/Interactable/Widget.hpp>
#include <Game/Interactable/NPC.hpp>

class POH
{
public:
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

    typedef enum PORTAL_NEXUS_ROOM
    {
        MOUNTED_DIGSITE_PENDANT,
        MOUNTED_XERICS_TALISMAN,
        PORTAL_NEXUS
    } PORTAL_NEXUS_ROOM;

    typedef enum SUPERIOR_GARDEN_ROOM
    {
        RESTORATION_POOL, // Special attack
        REVITALISATION_POOL, // Special attack, Run energy
        REJUVENATION_POOL, // Special attack, Run energy, Prayer points
        FANCY_REJUVENATION_POOL, // Special attack, Run energy, Prayer points, Reduced stats (excluding Hitpoints)
        ORNATE_REJUVENATION_POOL , // Special attack, Run energy, Prayer points, Reduced stats, Hitpoints
        SPIRIT_TREE,
        OBELISK,
        FAIRY_RING,
        SPIRIT_TREE_FAIRY_RING
    } SUPERIOR_GARDEN_ROOM;

    typedef enum ACHIEVEMENT_GALLERY_ROOM
    {
        BASIC_JEWELLERY_BOX, // Games necklace, Ring of dueling
        FANCY_JEWELLERY_BOX, // Skills necklace, Combat bracelet
        ORNATE_JEWELLERY_BOX, // Amulet of glory, Ring of wealth
        ANCIENT,
        LUNAR,
        DARK,
        OCCULT
    } ACHIEVEMENT_GALLERY_ROOM;

    typedef enum WORKSHOP_ROOM
    {
        ARMOUR_STAND
    } WORKSHOP_ROOM;

    typedef enum TELEPORT
    {
        // Portal nexus
        NEXUS_VARROCK,
        NEXUS_GRAND_EXCHANGE,
        NEXUS_LUMBRIDGE,
        NEXUS_FALADOR,
        NEXUS_CAMELOT,
        NEXUS_SEERS_VILLAGE,
        NEXUS_ARDOUGNE,
        NEXUS_WATCHTOWER,
        NEXUS_YANILLE,
        NEXUS_SENNTISTEN,
        NEXUS_MARIM,
        NEXUS_KHARYRLL,
        NEXUS_LUNAR_ISLE,
        NEXUS_KOUREND_CASTLE,
        NEXUS_WATERBIRTH_ISLAND,
        NEXUS_CARRALLANGAR,
        NEXUS_FISHING_GUILD,
        NEXUS_CATHERBY,
        NEXUS_ANNAKARL,
        NEXUS_GHORROCK,
        NEXUS_TROLL_STRONGHOLD,

        // Walls
        // - Digsite Pendant
        WALL_DIGSITE,
        WALL_FOSSIL_ISLAND,
        WALL_LITHKREN,
        // - Xeric's Talisman
        WALL_XERICS_LOOKOUT,
        WALL_XERICS_GLADE,
        WALL_XERICS_INFERNO,
        WALL_XERICS_HEART,
        WALL_XERICS_HONOUR,

        // Jewellery
        // - Ring of dueling
        JEWELLERY_DUAL_ARENA,
        JEWELLERY_CASTLE_WARS,
        JEWELLERY_CLAN_WARS,
        // - Games Necklace
        JEWELLERY_BURTHORPE,
        JEWELLERY_BARBARIAN_OUTPOST,
        JEWELLERY_CORPOREAL_BEAST,
        JEWELLERY_TEARS_OF_GUTHIX,
        JEWELLERY_WINTERTODT_CAMP,
        // - Combat bracelet
        JEWELLERY_WARRIORS_GUILD,
        JEWELLERY_CHAMPIONS_GUILD,
        JEWELLERY_MONASTERY,
        JEWELLERY_RANGING_GUILD,
        // - Skills necklace
        JEWELLERY_FISHING_GUILD,
        JEWELLERY_MINING_GUILD,
        JEWELLERY_CRAFTING_GUILD,
        JEWELLERY_COOKING_GUILD,
        JEWELLERY_WOODCUTTING_GUILD,
        JEWELLERY_FARMING_GUILD,
        // - Ring of Wealth
        JEWELLERY_MISCELLANIA,
        JEWELLERY_GRAND_EXCHANGE,
        JEWELLERY_FALADOR_PARK,
        JEWELLERY_DONDAKANS_ROCK,
        // - Amulet of Glory
        JEWELLERY_EDGEVILLE,
        JEWELLERY_KARAMJA,
        JEWELLERY_DRAYNOR_VILLAGE,
        JEWELLERY_AL_KHARID
    } TELEPORT;

    static POH_LOCATION GetLocation();

    static bool IsInside();
    static bool InBuildMode();
    static bool IsLoading();
    static bool IsHouseMenuOpen();

    static bool Enter(bool BuildMode = false); // Portal must be reachable
    static bool Exit(); // Does not support multi-plane

    static bool CallServant(); // Must be inside already
    static bool ToggleBuildMode(bool Toggle); // Must be inside already
    static bool OpenHouseMenu();
    static bool CloseHouseMenu();

    static Interactable::Widget GetHouseButtonWidget();
    static Interactable::Widget GetHouseMenuCloseWidget();
    static Interactable::Widget GetHouseMenuServantWidget();
    static Interactable::Widget GetHouseMenuBuildToggleWidget(); // only returns the widget not 'ticked'
    static Interactable::Widget GetHouseMenuTeleportToggleWidget(); // Only returns the widget not 'ticked'

    static Interactable::NPC GetServant();

    static Internal::Object Get(const PORTAL_NEXUS_ROOM& Obj);
    static Internal::Object Get(const SUPERIOR_GARDEN_ROOM& Obj);
    static Internal::Object Get(const ACHIEVEMENT_GALLERY_ROOM& Obj);
    static Internal::Object Get(const WORKSHOP_ROOM& Obj);

    // Used for "any" getting, eg { ORNATE_JEWELLERY_BOX, FANCY_JEWELLERY_BOX } will return whichever is found first
    // used for finding any supported room object for a teleport
    static Internal::Object Get(const std::vector<PORTAL_NEXUS_ROOM>& Objs);
    static Internal::Object Get(const std::vector<SUPERIOR_GARDEN_ROOM>& Objs);
    static Internal::Object Get(const std::vector<ACHIEVEMENT_GALLERY_ROOM>& Objs);
    static Internal::Object Get(const std::vector<WORKSHOP_ROOM>& Objs);

    static bool Has(const PORTAL_NEXUS_ROOM& Obj);
    static bool Has(const SUPERIOR_GARDEN_ROOM& Obj);
    static bool Has(const ACHIEVEMENT_GALLERY_ROOM& Obj);
    static bool Has(const WORKSHOP_ROOM& Obj);
    static bool Has(const TELEPORT& Teleport); // Requires the Portal nexus widget be opened before using for Nexus teleports

    static bool Teleport(const TELEPORT& Teleport, bool UseKeyboard = false);
    //static bool Repair();
    static bool Drink(const SUPERIOR_GARDEN_ROOM& Pool, bool Force = false);
    //static bool ChangeSpellbook();

private:
//Close button, Text, Widget
    static std::tuple<Interactable::Widget, std::string, Interactable::Widget> FindTeleportWidget(const TELEPORT& Teleport);
    static char FindTeleportWidgetKeyShortcut(const std::string& Text, const TELEPORT& Teleport);
    static Internal::Object GetTeleportObject(const TELEPORT& Teleport);
};

namespace Globals
{
    // Object names
    static const std::array<std::string, 3> OBJECTS_PORTAL_NEXUS_ROOM = { "Digsite Pendant", "Xeric's Talisman", "Portal Nexus" };
    static const std::array<std::string, 9> OBJECTS_SUPERIOR_GARDEN_ROOM =
            {
                "Pool of Restoration", "Pool of Revitalisation", "Pool of Rejuvenation", "Fancy rejuvenation pool", "Ornate rejuvenation pool",
                "Spirit tree", "Obelisk", "Fairy ring", "Spiritual Fairy Tree"
            };
    static const std::array<std::string, 7> OBJECTS_ACHIEVEMENT_GALLERY_ROOM =
            {
                "Basic Jewellery Box", "Fancy Jewellery Box", "Ornate Jewellery Box",
                "Ancient Altar", "Lunar Altar", "Dark Altar", "Altar of the Occult"
            };
    static const std::array<std::string, 1> OBJECTS_WORKSHOP_ROOM = { "Armour repair stand" };

    // Widgets
    constexpr Globals::WidgetDef WIDGET_PORTAL_NEXUS_CLOSE = { 17, 1, 13 };
    constexpr Globals::WidgetDef WIDGET_PORTAL_NEXUS_TELEPORTS_INTERACTABLE_WIDGET_PARENT = { 17, 13 }; // Used for interacting
    constexpr Globals::WidgetDef WIDGET_PORTAL_NEXUS_TELEPORTS_TEXTS_PARENT = { 17, 12 }; // Used for scanning text
    constexpr Globals::WidgetDef WIDGET_PORTAL_NEXUS_TELEPORTS_CONTAINER_BOUNDS = { 17, 9 }; // The bounds of this widget determine if the Teleport widget box is visible, used for scrolling

    constexpr Globals::WidgetDef WIDGET_PARCHMENT_TELEPORT_CLOSE = { 187, 4 };
    constexpr Globals::WidgetDef WIDGET_PARCHMENT_TELEPORT_OPTIONS = { 187, 3 }; // Used for Digsite Pendant/Xerics Talisman

    constexpr Globals::WidgetDef WIDGET_JEWELLERY_BOX_CLOSE = { 590, 1, 11 };
    constexpr Globals::WidgetDef WIDGET_JEWELLERY_BOX_RING_OF_DUELING = { 590, 2 };
    constexpr Globals::WidgetDef WIDGET_JEWELLERY_BOX_GAMES_NECKLACE = { 590, 3 };
    constexpr Globals::WidgetDef WIDGET_JEWELLERY_BOX_COMBAT_BRACELET = { 590, 4 };
    constexpr Globals::WidgetDef WIDGET_JEWELLERY_BOX_SKILLS_NECKLACE = { 590, 5 };
    constexpr Globals::WidgetDef WIDGET_JEWELLERY_BOX_RING_OF_WEALTH = { 590, 6 };
    constexpr Globals::WidgetDef WIDGET_JEWELLERY_AMULET_OF_GLORY = { 590, 7 };



    static const std::map<POH::TELEPORT, std::string> POH_TELEPORT_ACTIONS = // [Teleport ID, Action string to search widget text for]
            {
                    // Portal Nexus
                    { POH::NEXUS_VARROCK, "Varrock" },
                    { POH::NEXUS_GRAND_EXCHANGE, "Grand Exchange" },
                    { POH::NEXUS_LUMBRIDGE, "Lumbridge" },
                    { POH::NEXUS_FALADOR, "Falador" },
                    { POH::NEXUS_CAMELOT, "Camelot" },
                    { POH::NEXUS_SEERS_VILLAGE, "Seers' Village" },
                    { POH::NEXUS_ARDOUGNE, "Ardougne" },
                    { POH::NEXUS_WATCHTOWER, "Watchtower" },
                    { POH::NEXUS_YANILLE, "Yanille" },
                    { POH::NEXUS_SENNTISTEN, "Senntisten" },
                    { POH::NEXUS_MARIM, "Marim" },
                    { POH::NEXUS_KHARYRLL, "Kharyrll" },
                    { POH::NEXUS_LUNAR_ISLE, "Lunar Isle" },
                    { POH::NEXUS_KOUREND_CASTLE, "Kourend Castle" },
                    { POH::NEXUS_WATERBIRTH_ISLAND, "Waterbirth Island" },
                    { POH::NEXUS_CARRALLANGAR, "Carrallangar" },
                    { POH::NEXUS_FISHING_GUILD, "Fishing Guild" },
                    { POH::NEXUS_CATHERBY, "Catherby" },
                    { POH::NEXUS_ANNAKARL, "Annakarl" },
                    { POH::NEXUS_GHORROCK, "Ghorrock" },
                    { POH::NEXUS_TROLL_STRONGHOLD, "Troll Stronghold" },

                    // Walls
                    // - Digsite Pendant
                    { POH::WALL_DIGSITE, "Digsite" },
                    { POH::WALL_FOSSIL_ISLAND, "Fossil Island" },
                    { POH::WALL_LITHKREN, "Lithkren" },
                    // - Xeric's Talisman
                    { POH::WALL_XERICS_LOOKOUT, "Xeric's Look-out" },
                    { POH::WALL_XERICS_GLADE, "Xeric's Glade" },
                    { POH::WALL_XERICS_INFERNO, "Xeric's Inferno" },
                    { POH::WALL_XERICS_HEART, "Xeric's Heart" },
                    { POH::WALL_XERICS_HONOUR, "Xeric's Honour" },

                    // Jewellery
                    // - Ring of dueling
                    { POH::JEWELLERY_DUAL_ARENA,        "Duel Arena" },
                    { POH::JEWELLERY_CASTLE_WARS,       "Castle Wars" },
                    { POH::JEWELLERY_CLAN_WARS,         "Clan Wars" },
                    // - Games Necklace
                    { POH::JEWELLERY_BURTHORPE,         "Burthorpe" },
                    { POH::JEWELLERY_BARBARIAN_OUTPOST, "Barbarian Outpost" },
                    { POH::JEWELLERY_CORPOREAL_BEAST,   "Corporeal Beast" },
                    { POH::JEWELLERY_TEARS_OF_GUTHIX,   "Tears of Guthix" },
                    { POH::JEWELLERY_WINTERTODT_CAMP,   "Wintertodt Camp" },
                    // - Combat bracelet
                    { POH::JEWELLERY_WARRIORS_GUILD,    "Warriors' Guild" },
                    { POH::JEWELLERY_CHAMPIONS_GUILD,   "Champions' Guild" },
                    { POH::JEWELLERY_MONASTERY,         "Monastery" },
                    { POH::JEWELLERY_RANGING_GUILD,     "Ranging Guild" },
                    // - Skills necklace
                    { POH::JEWELLERY_FISHING_GUILD,     "Fishing Guild" },
                    { POH::JEWELLERY_MINING_GUILD,      "Mining Guild" },
                    { POH::JEWELLERY_CRAFTING_GUILD, "Crafting Guild" },
                    { POH::JEWELLERY_COOKING_GUILD, "Cooking Guild" },
                    { POH::JEWELLERY_WOODCUTTING_GUILD, "Woodcutting Guild" },
                    { POH::JEWELLERY_FARMING_GUILD, "Farming Guild" },
                    // - Ring of Wealth
                    { POH::JEWELLERY_MISCELLANIA, "Miscellania" },
                    { POH::JEWELLERY_GRAND_EXCHANGE, "Grand Exchange" },
                    { POH::JEWELLERY_FALADOR_PARK, "Falador Park" },
                    { POH::JEWELLERY_DONDAKANS_ROCK, "Dondakan's Rock" },
                    // - Amulet of Glory
                    { POH::JEWELLERY_EDGEVILLE, "Edgeville" },
                    { POH::JEWELLERY_KARAMJA, "Karamja" },
                    { POH::JEWELLERY_DRAYNOR_VILLAGE, "Draynor Village" },
                    { POH::JEWELLERY_AL_KHARID, "Al Kharid" },
            };


    // Map regions
    constexpr std::uint16_t MAP_REGION_POH = 7513;

    typedef struct POH_SERVANT
    {
        std::string Name;
        std::int32_t ID;
        std::uint32_t Capacity;
        std::uint32_t TripTime;
    } POH_SERVANT;

    static const std::map<std::int32_t, POH_SERVANT> POH_SERVANT_DEFS =
            {
                    { 221, { "Rick", 221, 6, 60 * 1000 } },
                    { 223, { "Maid", 223, 10, 30 * 1000 } },
                    { 225, { "Cook", 225, 16, 17 * 1000 } },
                    { 227, { "Butler", 227, 20, 12 * 1000 } },
                    { 229, { "Demon butler", 229, 26, 7 * 1000 } }
            };

    // Settings
    //const SettingsDef SETTING_POH_LOCATION      = { 738, 0, 0xF };
    const SettingsDef SETTING_POH_BUILD_MODE    = { 780 };
    const SettingsDef SETTING_POH_LOADING       = { 1021 };
    const SettingsDef SETTING_POH_TELEPORTTOGGLE = { 4744 }; // Varbit

/*    auto Varbit = Internal::GetVarbit(4744);
    int value = Settings::GetSetting(Varbit.GetSettingID());
    int lsb = Varbit.GetLSB();
    int msb = Varbit.GetMSB();
    int mask = (1 << ((msb - lsb) + 1)) - 1;
    DebugLog((int) ((value >> lsb) & mask));*/
}

#endif // TLIB_POH_HPP_INCLUDED