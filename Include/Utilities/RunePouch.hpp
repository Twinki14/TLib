#ifndef TLIB_RUNEPOUCH_HPP
#define TLIB_RUNEPOUCH_HPP

#include <vector>
#include <array>
#include <string>

class RunePouch
{
public:

    enum RUNES
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

    typedef struct Rune
    {
        RUNES Rune;
        std::uint32_t ID;
        std::string Name;
        std::uint32_t Amount;
    } Rune;

    static std::array<RunePouch::Rune, 3> GetRunes();
    static std::int32_t Count(const RUNES& Rune);
    static bool Contains(const RUNES& Rune, std::uint32_t Tolerence = 100);
    static bool Contains(const std::vector<RUNES>& Runes, std::uint32_t Tolerence = 100);
};

#endif //TLIB_RUNEPOUCH_HPP
