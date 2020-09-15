#include "../../Include/Utilities/StatTracker.hpp"
#include "../../Include/TScript.hpp"
#include <sstream>
#include <Core/Time.hpp>
#include <Game/Interfaces/Mainscreen.hpp>

namespace Stats
{
    namespace
    {
        inline std::map<Stats::SKILLS, Stats::Tracker> Trackers;
        inline std::mutex AccessTrackerLock;
    }

    bool HasTracker(const Stats::SKILLS& Skill)
    {
        return Trackers.count(Skill);
    }

    void AddTracker(const Stats::SKILLS& Skill)
    {
        std::lock_guard<std::mutex>Lock (AccessTrackerLock);
        Trackers[Skill] = Stats::Tracker(Skill);
    }

    Stats::Tracker& GetTracker(const Stats::SKILLS& Skill)
    {
        std::lock_guard<std::mutex> Lock (AccessTrackerLock);
        Trackers[Skill].SetStartInfo();
        return Trackers[Skill];
    }

    Tracker::Tracker()
    {
        this->Skill = Stats::ALL;
        this->StartingXP = 0;
        this->StartingLevel = 0;
    }

    Tracker::Tracker(const Stats::SKILLS& Skill)
    {
        this->Skill = Skill;
        this->StartingXP = 0;
        this->StartingLevel = 0;
        this->SetStartInfo();
    }

    Tracker::Tracker(const Tracker& Tracker)
    {
        this->Skill = Tracker.GetSkill();
        this->StartingXP = 0;
        this->StartingLevel = 0;
        this->SetStartInfo();
    }

    std::string Tracker::GetSkillName(bool PascalCase) const
    {
        if (!PascalCase)
            return Globals::SKILL_NAMES[Skill];
        else
        {
            bool First = true;
            std::string Name = Globals::SKILL_NAMES[Skill];
            for (auto& C : Name)
            {
                if (First)
                {
                    C = std::toupper(C);
                    First = false;
                } else
                    C = std::tolower(C);
            }
            return Name;
        }
    }

    Stats::SKILLS Tracker::GetSkill()           const { return Skill; }
    std::int32_t Tracker::GetStartingXP()       const { return StartingXP; }
    std::int32_t Tracker::GetStartingLevel()    const { return StartingLevel; }

    std::int32_t Tracker::GetCurrentXP()    const { return Stats::GetExperience(this->GetSkill()); }
    std::int32_t Tracker::GetGainedXP()     const { if (this->GetStartingXP() <= 0) return 0; return Stats::GetExperience(this->GetSkill()) - this->GetStartingXP(); }
    std::int64_t Tracker::GetXPPerHour()    const { if (this->GetStartingXP() <= 0) return 0; return Script::Tools::ToHour(this->GetStartingXP(), Stats::GetExperience(this->GetSkill())); }

    std::int32_t Tracker::GetCurrentLevel()  const { return Stats::GetRealLevel(this->GetSkill()); }
    std::int32_t Tracker::GetGainedLevels()  const { if (this->GetStartingLevel() <= 0) return 0; return Stats::GetRealLevel(this->GetSkill()) - this->GetStartingLevel(); }
    std::int32_t Tracker::GetLevelsPerHour() const { if (this->GetStartingLevel() <= 0) return 0; return Script::Tools::ToHour(this->GetStartingLevel(), Stats::GetRealLevel(this->GetSkill()));}

    std::int64_t Tracker::GetNextLevelETA() const { return (float)(((float)Stats::GetExperienceTo(this->GetSkill(), Stats::GetRealLevel(this->GetSkill()) + 1)) / (float) this->GetXPPerHour()) * (float) 3600000; }
    std::int64_t Tracker::GetMaxLevelETA()  const { return (float)(((float)Stats::GetExperienceTo(this->GetSkill(), 99)) / (float) GetXPPerHour()) * (float) 3600000; }

    std::string Tracker::GetAsStatus(bool Console, bool TTL, bool TTM) const
    {
        std::stringstream Status;
        if (!Console)
            Status << GetSkillName(true) << " [" << GetCurrentLevel() << "] - ";
        else
            Status << "[" << GetSkillName(true) << "] > ";

        Status << "Gained " << Script::Tools::FormatRunescapeGold(GetGainedXP()) << " XP";
        Status << " [" << Script::Tools::FormatRunescapeGold(GetXPPerHour()) << " p/h]";

        if (TTL || TTM)
        {
            auto CurrentLevel = GetCurrentLevel();
            if ((TTL && CurrentLevel < 99) || CurrentLevel == 98) Status << " - " << MillisToHumanShort(GetNextLevelETA()) << " till " << CurrentLevel + 1;
            if (TTM && CurrentLevel < 98) Status << " - " << MillisToHumanShort(GetMaxLevelETA()) << " till 99";
        }

        return Status.str();
    }

    void Tracker::SetStartInfo()
    {
        bool XP = this->StartingXP <= 0;
        bool Level = this->StartingLevel <= 0;

        if (Skill != SKILLS::ALL && (XP || Level))
        {
            if (Mainscreen::GetState() == Mainscreen::PLAYING)
            {
                if (XP) this->StartingXP = Stats::GetExperience(Skill);
                if (Level) this->StartingLevel = Stats::GetRealLevel(Skill);
            }
        }
    }
}