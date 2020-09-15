#ifndef TLIB_RUNES_HPP_INCLUDED
#define TLIB_RUNES_HPP_INCLUDED

#include <vector>
#include <cstdint>
#include <Core/Globals.hpp>

namespace Runes
{
    enum RUNE
    {
        AIR = 1,
        WATER,
        EARTH,
        FIRE,
        MIND,
        CHAOS,
        DEATH,
        BLOOD,
        COSMIC,
        NATURE,
        LAW,
        BODY,
        SOUL,
        ASTRAL,
        MIST,
        MUD,
        DUST,
        LAVA,
        STEAM,
        SMOKE,
        WRATH
    };

    enum SOURCE
    {
        INVENTORY = 1 << 0,
        EQUIPMENT = 1 << 1,
        POUCH = 1 << 2
    };

    std::int32_t Count(Runes::RUNE Rune);
    std::int32_t Count(Runes::RUNE Rune, std::int32_t Source);

    bool Contains(Runes::RUNE Rune);
    bool Contains(Runes::RUNE Rune, std::int32_t Source);
    bool Contains(const std::vector<Runes::RUNE>& Runes);
    bool Contains(const std::vector<Runes::RUNE>& Runes, std::int32_t Source);

    namespace Pouch
    {
        std::vector<Runes::RUNE> GetRunes();
        std::vector<std::int32_t> GetRuneIDs();
        std::vector<std::int32_t> GetRuneAmounts();
        std::int32_t Count(RUNE Rune);
    }
}

namespace Globals
{
    typedef struct RuneProviderDef
    {
        std::int32_t ItemID;
        std::string ItemName;
        std::int32_t Source; // uses the SOURCE bit flags in Runes::
        std::int32_t ProvidedAmount; // -1 = infinite
        //std::vector<Runes::RUNE> ProvidedRunes;
    } RuneProviderDef;

    typedef struct RuneDef
    {
        std::int32_t ID;
        std::string Name;
        std::vector<RuneProviderDef> Providers;
    } RuneDef;

    inline const std::map<Runes::RUNE, RuneDef> RuneDefs =
    {
            { Runes::AIR, { 556, "Air rune",
                                  {
                                          { 556, "Air rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                          { 4695, "Mist rune",  Runes::INVENTORY | Runes::POUCH, 1 },
                                          { 4696, "Dust rune",  Runes::INVENTORY | Runes::POUCH, 1 },
                                          { 4697, "Smoke rune", Runes::INVENTORY | Runes::POUCH, 1 },
                                          { 1381, "Staff of air",           Runes::EQUIPMENT, -1 },
                                          { 1397, "Air battlestaff",        Runes::EQUIPMENT, -1 },
                                          { 1405, "Mystic air staff",       Runes::EQUIPMENT, -1 },
                                          { 20736, "Dust battlestaff",      Runes::EQUIPMENT, -1 },
                                          { 20739, "Mystic dust staff",     Runes::EQUIPMENT, -1 },
                                          { 20730, "Mist battlestaff",      Runes::EQUIPMENT, -1 },
                                          { 20733, "Mystic mist staff",     Runes::EQUIPMENT, -1 },
                                          { 11998, "Smoke battlestaff",     Runes::EQUIPMENT, -1 },
                                          { 12000, "Mystic smoke staff",    Runes::EQUIPMENT, -1 }
                                  }
                          }
            },

            { Runes::WATER, { 555, "Water rune",
                                    {
                                            { 555, "Water rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 4695, "Mist rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 4698, "Mud rune",     Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 4694, "Steam rune",   Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 1383, "Staff of water",       Runes::EQUIPMENT, -1 },
                                            { 1395, "Water battlestaff",    Runes::EQUIPMENT, -1 },
                                            { 1403, "Mystic water staff",   Runes::EQUIPMENT, -1 },
                                            { 6562, "Mud battlestaff",      Runes::EQUIPMENT, -1 },
                                            { 6563, "Mystic mud staff",     Runes::EQUIPMENT, -1 },
                                            { 11787, "Steam battlestaff",   Runes::EQUIPMENT, -1 },
                                            { 11789, "Mystic steam staff",  Runes::EQUIPMENT, -1 },
                                            { 12795, "Steam battlestaff",   Runes::EQUIPMENT, -1 },
                                            { 12796, "Mystic steam staff",  Runes::EQUIPMENT, -1 },
                                            { 20730, "Mist battlestaff",    Runes::EQUIPMENT, -1 },
                                            { 20733, "Mystic mist staff",   Runes::EQUIPMENT, -1 }
                                    }
                            }
            },

            { Runes::EARTH, { 557, "Earth rune",
                                    {
                                            { 557, "Earth rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 4698, "Mud rune",     Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 4696, "Dust rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 4699, "Lava rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 1385, "Staff of earth",       Runes::EQUIPMENT, -1 },
                                            { 1399, "Earth battlestaff",    Runes::EQUIPMENT, -1 },
                                            { 1407, "Mystic earth staff",   Runes::EQUIPMENT, -1 },
                                            { 3053, "Lava battlestaff",     Runes::EQUIPMENT, -1 },
                                            { 3054, "Mystic lava staff",    Runes::EQUIPMENT, -1 },
                                            { 21198, "Lava battlestaff",    Runes::EQUIPMENT, -1 }, // cosmetic version
                                            { 21200, "Mystic lava staff",   Runes::EQUIPMENT, -1 }, // cosmetic version
                                            { 6562, "Mud battlestaff",      Runes::EQUIPMENT, -1 },
                                            { 6563, "Mystic mud staff",     Runes::EQUIPMENT, -1 },
                                            { 20736, "Dust battlestaff",    Runes::EQUIPMENT, -1 },
                                            { 20739, "Mystic dust staff",   Runes::EQUIPMENT, -1 },
                                    }
                            }
            },

            { Runes::FIRE, { 554, "Fire rune",
                                   {
                                           { 554, "Fire rune",      Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 4699, "Lava rune",     Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 4694, "Steam rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 4697, "Smoke rune",    Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 1387, "Staff of fire",         Runes::EQUIPMENT, -1 },
                                           { 1393, "Fire battlestaff",      Runes::EQUIPMENT, -1 },
                                           { 1401, "Mystic fire staff",     Runes::EQUIPMENT, -1 },
                                           { 3053, "Lava battlestaff",      Runes::EQUIPMENT, -1 },
                                           { 3054, "Mystic lava staff",     Runes::EQUIPMENT, -1 },
                                           { 21198, "Lava battlestaff",     Runes::EQUIPMENT, -1 },
                                           { 21200, "Mystic lava staff",    Runes::EQUIPMENT, -1 },
                                           { 11787, "Steam battlestaff",    Runes::EQUIPMENT, -1 },
                                           { 11789, "Mystic steam staff",   Runes::EQUIPMENT, -1 },
                                           { 12795, "Steam battlestaff",    Runes::EQUIPMENT, -1, }, // cosmetic version=
                                           { 12796, "Mystic steam staff",   Runes::EQUIPMENT, -1 }, // cosmetic version
                                           { 11998, "Smoke battlestaff",    Runes::EQUIPMENT, -1 },
                                           { 12000, "Mystic smoke staff",   Runes::EQUIPMENT, -1 },
                                   }
                           }
            },

            { Runes::MIND,      { 558, "Mind rune",     { { 558, "Mind rune",   Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::CHAOS,     { 562, "Chaos rune",    { { 562, "Chaos rune",  Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::DEATH,     { 560, "Death rune",    { { 560, "Death rune",  Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::BLOOD,     { 565, "Blood rune",    { { 565, "Blood rune",  Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::COSMIC,    { 564, "Cosmic rune",   { { 564, "Cosmic rune", Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::NATURE,    { 561, "Nature rune",   { { 561, "Nature rune", Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::LAW,       { 563, "Law rune",      { { 563, "Law rune",    Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::BODY,      { 559, "Body rune",     { { 559, "Body rune",   Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::SOUL,      { 566, "Soul rune",     { { 566, "Soul rune",   Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::ASTRAL,    { 9075, "Astral rune",  { { 9075, "Astral rune", Runes::INVENTORY | Runes::POUCH, 1 } } } },
            { Runes::WRATH,     { 21880, "Wrath rune",  { { 21880, "Wrath rune", Runes::INVENTORY | Runes::POUCH, 1 } } } },

            { Runes::MIST, { 4695, "Mist rune",
                                   {
                                           { 4695, "Mist rune",             Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 20730, "Mist battlestaff",     Runes::EQUIPMENT, -1 },
                                           { 20733, "Mystic mist staff",    Runes::EQUIPMENT, -1 },
                                   }
                           }
            },

            { Runes::DUST, { 4696, "Dust rune",
                                   {
                                           { 4696, "Dust rune",             Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 20736, "Dust battlestaff",     Runes::EQUIPMENT, -1 },
                                           { 20739, "Mystic dust staff",    Runes::EQUIPMENT, -1 },
                                   }
                           }
            },

            { Runes::MUD, { 4698, "Mud rune",
                                  {
                                          { 4698, "Mud rune",           Runes::INVENTORY | Runes::POUCH, 1 },
                                          { 6562, "Mud battlestaff",    Runes::EQUIPMENT, -1 },
                                          { 6563, "Mystic mud staff",   Runes::EQUIPMENT, -1 },
                                  }
                          }
            },

            { Runes::SMOKE, { 4697, "Smoke rune",
                                    {
                                            { 4697, "Smoke rune",           Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 11998, "Smoke battlestaff",   Runes::EQUIPMENT, -1 },
                                            { 12000, "Mystic smoke staff",  Runes::EQUIPMENT, -1 },
                                    }
                            }
            },


            { Runes::STEAM, { 4694, "Steam rune",
                                    {
                                            { 4694, "Steam rune",       Runes::INVENTORY | Runes::POUCH, 1 },
                                            { 11787, "Steam battlestaff",   Runes::EQUIPMENT, -1 },
                                            { 11789, "Mystic steam staff",  Runes::EQUIPMENT, -1 },
                                            { 12795, "Steam battlestaff",   Runes::EQUIPMENT, -1, }, // cosmetic version=
                                            { 12796, "Mystic steam staff",  Runes::EQUIPMENT, -1 }, // cosmetic version
                                    }
                            }
            },


            { Runes::LAVA, { 4699, "Lava rune",
                                   {
                                           { 4699, "Lava rune",         Runes::INVENTORY | Runes::POUCH, 1 },
                                           { 3053, "Lava battlestaff",          Runes::EQUIPMENT, -1 },
                                           { 3054, "Mystic lava staff",         Runes::EQUIPMENT, -1 },
                                           { 21198, "Lava battlestaff",         Runes::EQUIPMENT, -1 }, // cosmetic version
                                           { 21200, "Mystic lava staff",        Runes::EQUIPMENT, -1 }, // cosmetic version
                                   }
                           }
            },

    };

/*    inline const std::array<RuneDef, 15> RuneDefs = // Ordered by Runes::RUNE enum
    {
            {
                    { 556, "Air rune" },
                    { 557, "Earth rune" },
                    { 554, "Fire rune" },
                    { 558, "Mind rune" },
                    { 562, "Chaos rune" },
                    { 560, "Death rune" },
                    { 565, "Blood rune" },
                    { 564, "Cosmic rune" },
                    { 561, "Nature rune" },
                    { 563, "Law rune" },
                    { 559, "Body rune" },
                    { 566, "Soul rune" },
                    { 9075, "Astral rune" },
                    { 21880, "Wrath rune" }

*//*                    { 4695, "Mist rune" },
                    { 4698, "Mud rune" },
                    { 4696, "Dust rune" },
                    { 4699, "Lava rune" },
                    { 4694, "Steam rune" },
                    { 4697, "Smoke rune" }*//*
            }

    };*/


/*    inline const std::array<RuneProviderDef, 49> RuneProvidersDefs = // Not ordered by anything
    {
        {
            // Regular runes found in inventory/rune pouch
            { 556, "Air rune",      Runes::INVENTORY | Runes::POUCH, 1, { Runes::AIR } },
            { 555, "Water rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::WATER } },
            { 557, "Earth rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::EARTH } },
            { 554, "Fire rune",     Runes::INVENTORY | Runes::POUCH, 1, { Runes::FIRE } },
            { 558, "Mind rune",     Runes::INVENTORY | Runes::POUCH, 1, { Runes::MIND } },
            { 562, "Chaos rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::CHAOS } },
            { 560, "Death rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::DEATH } },
            { 565, "Blood rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::BLOOD } },
            { 564, "Cosmic rune",   Runes::INVENTORY | Runes::POUCH, 1, { Runes::COSMIC } },
            { 561, "Nature rune",   Runes::INVENTORY | Runes::POUCH, 1, { Runes::NATURE } },
            { 563, "Law rune",      Runes::INVENTORY | Runes::POUCH, 1, { Runes::LAW } },
            { 559, "Body rune",     Runes::INVENTORY | Runes::POUCH, 1, { Runes::BODY } },
            { 566, "Soul rune",     Runes::INVENTORY | Runes::POUCH, 1, { Runes::SOUL } },
            { 9075, "Astral rune",  Runes::INVENTORY | Runes::POUCH, 1, { Runes::ASTRAL } },
            { 21880, "Wrath rune",  Runes::INVENTORY | Runes::POUCH, 1, { Runes::WRATH } },

            { 4695, "Mist rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::AIR, Runes::WATER } }, // Runes::MIST
            { 4698, "Mud rune",     Runes::INVENTORY | Runes::POUCH, 1, { Runes::WATER, Runes::EARTH } }, // Runes::MUD
            { 4696, "Dust rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::AIR, Runes::EARTH } }, // Runes::DUST
            { 4699, "Lava rune",    Runes::INVENTORY | Runes::POUCH, 1, { Runes::FIRE, Runes::EARTH } }, // Runes::LAVA
            { 4694, "Steam rune",   Runes::INVENTORY | Runes::POUCH, 1, { Runes::WATER, Runes::FIRE } }, // Runes::STEAM
            { 4697, "Smoke rune",   Runes::INVENTORY | Runes::POUCH, 1, { Runes::AIR, Runes::FIRE } }, // Runes::SMOKE

            { 1383, "Staff of water",       Runes::EQUIPMENT, -1, { Runes::WATER } },
            { 1395, "Water battlestaff",    Runes::EQUIPMENT, -1, { Runes::WATER } },
            { 1403, "Mystic water staff",   Runes::EQUIPMENT, -1, { Runes::WATER } },

            { 1385, "Staff of earth",       Runes::EQUIPMENT, -1, { Runes::EARTH } },
            { 1399, "Earth battlestaff",    Runes::EQUIPMENT, -1, { Runes::EARTH } },
            { 1407, "Mystic earth staff",   Runes::EQUIPMENT, -1, { Runes::EARTH } },

            { 1387, "Staff of fire",        Runes::EQUIPMENT, -1, { Runes::FIRE } },
            { 1393, "Fire battlestaff",     Runes::EQUIPMENT, -1, { Runes::FIRE } },
            { 1401, "Mystic fire staff",    Runes::EQUIPMENT, -1, { Runes::FIRE } },

            { 3053, "Lava battlestaff",     Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::FIRE } }, // Runes::LAVA
            { 3054, "Mystic lava staff",    Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::FIRE } }, // Runes::LAVA
            { 21198, "Lava battlestaff",    Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::FIRE } }, // cosmetic version // Runes::LAVA
            { 21200, "Mystic lava staff",   Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::FIRE } }, // cosmetic version // Runes::LAVA

            { 6562, "Mud battlestaff",      Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::WATER } }, // Runes::MUD
            { 6563, "Mystic mud staff",     Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::WATER } }, // Runes::MUD

            { 11787, "Steam battlestaff",   Runes::EQUIPMENT, -1, { Runes::FIRE, Runes::WATER } }, // Runes::STEAM
            { 11789, "Mystic steam staff",  Runes::EQUIPMENT, -1, { Runes::FIRE, Runes::WATER } },  // Runes::STEAM
            { 12795, "Steam battlestaff",   Runes::EQUIPMENT, -1, { Runes::FIRE, Runes::WATER } }, // cosmetic version // Runes::STEAM
            { 12796, "Mystic steam staff",  Runes::EQUIPMENT, -1, { Runes::FIRE, Runes::WATER } }, // cosmetic version // Runes::STEAM

            { 11998, "Smoke battlestaff",   Runes::EQUIPMENT, -1, { Runes::FIRE, Runes::AIR } }, // Runes::SMOKE
            { 12000, "Mystic smoke staff",  Runes::EQUIPMENT, -1, { Runes::FIRE, Runes::AIR } }, // Runes::SMOKE

            { 20730, "Mist battlestaff",    Runes::EQUIPMENT, -1, { Runes::WATER, Runes::AIR } }, // Runes::MIST
            { 20733, "Mystic mist staff",   Runes::EQUIPMENT, -1, { Runes::WATER, Runes::AIR } }, // Runes::MIST

            { 20736, "Dust battlestaff",    Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::AIR } }, // Runes::DUST
            { 20739, "Mystic dust staff",   Runes::EQUIPMENT, -1, { Runes::EARTH, Runes::AIR } }, // Runes::DUST
        }
    };*/
}

#endif // TLIB_RUNES_HPP_INCLUDED