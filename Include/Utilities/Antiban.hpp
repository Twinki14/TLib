#ifndef TLIB_ANTIBAN_HPP_INCLUDED
#define TLIB_ANTIBAN_HPP_INCLUDED

#include <map>
#include <vector>
#include <Core/Types/Timer.hpp>
#include <Core/Types/Countdown.hpp>
#include <Core/Globals.hpp>
#include <Game/Tools/Antiban.hpp>

namespace Antiban
{
    class Task
    {
    public:

        Task();
        Task(std::int32_t EveryMean, double EveryDeviation, double Chance, bool (*Func)() = nullptr, std::uint64_t FuncCooldownTime = -1);
        //Task(std::int32_t EveryMean, double EveryDeviation, bool (*CheckFunc)(), bool (*Func)(), std::uint64_t FuncCooldownTime = -1);
        bool Run(bool Force = false);
        void Reset();

        operator bool() const;

    private:

        std::int32_t EveryMean;
        double EveryDeviation;

        double Chance = -1.00;
        bool (*Func)() = nullptr;
        //bool (*CheckFunc)() = nullptr;

        std::uint64_t FuncCooldownTime;
        Countdown FuncCooldown;

        std::int32_t Next;
        Timer T;
    };

    extern std::map<std::string, Antiban::Task> Tasks;
    void AddTask(const std::string& Key, const Antiban::Task& Task);
    void AddTasks(const std::vector<std::string>& Keys, const std::vector<Antiban::Task>& Tasks);

    bool RunTask(const std::string& Key, bool Force = false);
    bool RunTasks(); // Returns on first task ran
    bool RunTasks(const std::vector<std::string>& Tasks, bool Force = false); // Returns on first task ran

    void ResetTask(const std::string& Key);
    void ResetTasks(const std::vector<std::string>& Tasks);

    bool TwitchCamera();
    bool SmallCamera();
    bool LongCamera();
    bool DestinationCamera();

    bool TabOut(bool MouseOff, std::int32_t Time, bool GainBack);
    bool TabOut_Quick(); // ~5 second tabout - Intended to replicate replying on Discord/Social media
    bool TabOut_Medium(); // ~20-30 second tabout - Intended to replicate getting a drink, letting dogs out, finding a new youtube video
    bool TabOut_Long(); // ~1-2 minute tabout - Intended to replicate a bathroom break/getting a quick meal/snack

    bool AFK(bool TabOut, std::int32_t Time, bool MouseOff = true, bool GainBack = true);
    bool AFK_Quick(); // ~5 second afk - Intended to replicate replying on Discord/Social media
    bool AFK_Medium(); // ~20-30 second afk - Intended to replicate getting a drink, letting dogs out, finding a new youtube video
    bool AFK_Long(); // ~1-2 minute afk - Intended to replicate a bathroom break/getting a quick meal/snack

    // Delay = (Base * (ProfilePassiveness + 1.0f)) * Multiplier)
    // Multiplier will multiply the delay if ChanceToMultiply passes, intended to rarely or often generate even longer/shorter delays based on ChanceToMultiply
    // If the delay is above Max, the delay will become Max, before going into NormalRandom
    // Delay is then tossed into NormalRandom with a deviation of 0.10 before returning for more skewing
    // Note: The NormalRandom will not consider Max, if the calculation hits Max, and NormalRandom skews it further, the return CAN be above Max
    std::int32_t GenerateDelayFromPassivity(std::int32_t Min, std::int32_t Max, double Multiplier = 1.0, double ChanceToMultiply = 0.10);
    void DelayFromPassivity(std::int32_t Min, std::int32_t Max, double Multiplier = 1.0, double ChanceToMultiply = 0.10);

    namespace Tracker
    {
        inline std::atomic<std::uint32_t> TimesMovedCamera = 0;
        inline std::atomic<std::uint32_t> TimesTwitchedCamera = 0;
        inline std::atomic<std::uint32_t> TimesSmallCamera = 0;
        inline std::atomic<std::uint32_t> TimesLongCamera = 0;
        inline std::atomic<std::uint32_t> TimesDestinationCamera = 0;

        inline std::atomic<std::uint64_t> TimeSpentTabbedOut = 0;
        inline std::atomic<std::uint32_t> TimesTabbedOut = 0;
        inline std::atomic<std::uint32_t> TimesTabbedOut_Quick = 0;
        inline std::atomic<std::uint32_t> TimesTabbedOut_Medium = 0;
        inline std::atomic<std::uint32_t> TimesTabbedOut_Long = 0;

        inline std::atomic<std::uint64_t> TimeSpentAFK = 0;
        inline std::atomic<std::uint32_t> TimesAFKed = 0;
        inline std::atomic<std::uint32_t> TimesAFK_Quick = 0;
        inline std::atomic<std::uint32_t> TimesAFK_Medium = 0;
        inline std::atomic<std::uint32_t> TimesAFK_Long = 0;
    }
};

namespace Globals
{
    const WidgetDef WIDGET_XP_TRACKER_BAR = { 122, 13 };
}

#endif // TLIB_ANTIBAN_HPP_INCLUDED