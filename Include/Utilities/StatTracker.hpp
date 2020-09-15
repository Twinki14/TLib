#ifndef TLIB_STATTRACKER_HPP_INCLUDED
#define TLIB_STATTRACKER_HPP_INCLUDED

#include <Game/Interfaces/GameTabs/Stats.hpp>
#include <Core/Globals.hpp>

namespace Globals
{
    const std::vector<std::string> SKILL_NAMES = {
            "ATTACK", "DEFENCE", "STRENGTH", "HITPOINTS", "RANGE", "PRAYER", "MAGIC", "COOKING", "WOODCUTTING", "FLETCHING", "FISHING", "FIREMAKING",
            "CRAFTING", "SMITHING", "MINING", "HERBLORE", "AGILITY", "THIEVING", "SLAYER", "FARMING", "RUNECRAFT", "HUNTER", "CONSTRUCTION", "ALL" };
}

namespace Stats
{
    class Tracker
    {
    public:
        Tracker();
        Tracker(const Stats::SKILLS& Skill);
        Tracker(const Tracker& Tracker);

        std::string GetSkillName(bool PascalCase = false) const;

        Stats::SKILLS GetSkill() const;
        std::int32_t GetStartingXP() const;
        std::int32_t GetStartingLevel() const;

        std::int32_t GetCurrentXP() const;
        std::int32_t GetGainedXP() const;
        std::int64_t GetXPPerHour() const;

        std::int32_t GetCurrentLevel() const;
        std::int32_t GetGainedLevels() const;
        std::int32_t GetLevelsPerHour() const;

        std::int64_t GetNextLevelETA() const;
        std::int64_t GetMaxLevelETA() const;

        std::string GetAsStatus(bool Console = false, bool TTL = false, bool TTM = false) const;
        void SetStartInfo();

        ~Tracker() = default;
    private:
        Stats::SKILLS Skill;
        std::int32_t StartingXP;
        std::int32_t StartingLevel;
    };

    bool HasTracker(const Stats::SKILLS& Skill);
    void AddTracker(const Stats::SKILLS& Skill);
    Stats::Tracker& GetTracker(const Stats::SKILLS& Skill);
}

#endif // TLIB_STATTRACKER_HPP_INCLUDED