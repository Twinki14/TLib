#include "../../Include/TScript.hpp"
#include "../../Include/Utilities/Antiban.hpp"
#include "../../Include/Utilities/TProfile.hpp"
#include "../../Include/Utilities/BackgroundTask/CameraTask.hpp"
#include <Core/Math.hpp>
#include <Core/Input.hpp>
#include <Game/Tools/Camera.hpp>
#include <Game/Interfaces/Minimap.hpp>

namespace Antiban
{
    Task::Task()
    {
        this->EveryMean = 0;
        this->EveryDeviation = 0.00;
        this->Chance = 0;
        //this->CheckFunc = nullptr;
        this->Func = nullptr;
        this->Next = 0;
        this->FuncCooldownTime = 0;
        this ->FuncCooldown = Countdown(0);
        this->T = Timer(false);
    }

    Task::Task(std::int32_t EveryMean, double EveryDeviation, double Chance, bool (*Func)(), std::uint64_t FuncCooldownTime)
    {
        this->EveryMean = EveryMean;
        this->EveryDeviation = EveryDeviation;
        this->Chance = Chance;
        //this->CheckFunc = nullptr;
        this->Func = Func;
        this->FuncCooldownTime = FuncCooldownTime;
        this->FuncCooldown = Countdown(this->FuncCooldownTime);
        this->Next = NormalRandom(this->EveryMean, this->EveryMean * this->EveryDeviation);
        this->T = Timer(false);
    }

    /**
    Task::Task(std::int32_t EveryMean, double EveryDeviation, bool (*CheckFunc)(), bool (*Func)(), std::uint64_t FuncCooldownTime)
    {
        this->EveryMean = EveryMean;
        this->EveryDeviation = EveryDeviation;
        this->Chance = 0.00;
        this->CheckFunc = CheckFunc;
        this->Func = Func;
        this ->FuncCooldownTime = FuncCooldownTime;
        this ->FuncCooldown = Countdown(0);
        this->Next = NormalRandom(this->EveryMean, this->EveryMean * this->EveryDeviation);
        this->T = Timer(false);
    }
     **/

    bool Task::Run(bool Force)
    {
        if (Force || T.GetTimeElapsed() >= Next)
        {
            if (Chance > 0.00 && UniformRandom() <= Chance)
            {
                if (Func)
                {
                    if (FuncCooldownTime > 0)
                    {
                        //DebugLog  << FuncCooldown << std::endl;
                        if (FuncCooldown.IsFinished())
                        {
                            FuncCooldown = Countdown(FuncCooldownTime);
                            Func();
                        }
                    } else
                        Func();
                }
                Reset();
                return true;
            }
            Reset();
        } else
        {
            if (T.Paused())
                T.Restart();
        }
        return false;
    }

    void Task::Reset()
    {
        T.Restart();
        Next = NormalRandom(EveryMean, EveryMean * EveryDeviation);
    }

    Task::operator bool() const
    {
        return (EveryMean > 0 && Chance > 0);
    }

    std::map<std::string, Antiban::Task> Tasks;

    void AddTask(const std::string& Key, const Task& Task)
    {
        if (!Antiban::Tasks.count(Key))
            Antiban::Tasks[Key] = Task;
    }

    void AddTasks(const std::vector<std::string>& Keys, const std::vector<Task>& Tasks)
    {
        if (Keys.size() != Tasks.size())
            return;

        for (std::uint32_t I = 0; I < Keys.size(); I++)
            if (!Antiban::Tasks.count(Keys[I]) && Tasks[I])
                Antiban::Tasks[Keys[I]] = Tasks[I];
    }

    bool RunTask(const std::string& Key, bool Force)
    {
        if (Antiban::Tasks.count(Key))
            return Antiban::Tasks[Key].Run(Force);
        return false;
    }

    bool RunTasks()
    {
        for (auto& [K, T] : Antiban::Tasks)
            if (T.Run(false))
                return true;
        return false;
    }

    bool RunTasks(const std::vector<std::string>& Keys, bool Force)
    {
        for (const auto& K : Keys)
            if (Antiban::Tasks.count(K))
                if (Antiban::Tasks[K].Run(Force))
                    return true;
        return false;
    }

    void ResetTask(const std::string& Key)
    {
        if (Antiban::Tasks.count(Key))
            Antiban::Tasks[Key].Reset();
    }

    void ResetTasks(const std::vector<std::string>& Keys)
    {
        for (const auto& K : Keys)
            if (Antiban::Tasks.count(K))
                Antiban::Tasks[K].Reset();
    }

    bool TwitchCamera()
    {
        std::int32_t NewAngle = -1;
        std::int32_t NewPitch = -1;

        std::int32_t AngleChange = NormalRandom(45, 45 * 0.15);
        if (UniformRandom() <= 0.50) AngleChange = NormalRandom(-45, -45 * 0.15);
        NewAngle = (std::int32_t) Camera::GetAngle() + AngleChange;
        if (NewAngle < 0) NewAngle = 360 + AngleChange;

        if (UniformRandom() <= 0.25)
        {
            const auto CurrentPitch = Camera::GetPitch();
            NewPitch = CurrentPitch + NormalRandom(15, 15 * 0.05);
            if (UniformRandom() <= 0.50 && CurrentPitch > (Camera::MIDDLE + 25))
                NewPitch = CurrentPitch + NormalRandom(-15, -15 * 0.05);
        }

        Script::SetStatus("Twitching camera");
        Antiban::Tracker::TimesMovedCamera++;
        Antiban::Tracker::TimesTwitchedCamera++;
        CameraTask::Start( { NewAngle, NewPitch } );
        return true;
    }

    bool SmallCamera()
    {
        std::int32_t NewAngle = -1;
        std::int32_t NewPitch = -1;

        std::int32_t AngleChange = NormalRandom(60, 60 * 0.15);
        if (UniformRandom() <= 0.50) AngleChange = NormalRandom(-60, -60 * 0.15);
        NewAngle = (std::int32_t) Camera::GetAngle() + AngleChange;
        if (NewAngle < 0) NewAngle = 364 + AngleChange;


        if (UniformRandom() <= 0.15)
        {
            const auto CurrentPitch = Camera::GetPitch();
            NewPitch = CurrentPitch + NormalRandom(15, 15 * 0.05);
            if (UniformRandom() <= 0.50 && CurrentPitch > (Camera::MIDDLE + 25))
                NewPitch = CurrentPitch + NormalRandom(-15, -15 * 0.05);
        }

        Script::SetStatus("Moving camera a small amount");
        Antiban::Tracker::TimesMovedCamera++;
        Antiban::Tracker::TimesSmallCamera++;
        CameraTask::Start( { NewAngle, NewPitch } );
        return true;
    }

    bool LongCamera()
    {
        std::int32_t NewAngle = -1;
        std::int32_t NewPitch = -1;

        std::int32_t AngleChange = NormalRandom(130, 130 * 0.25);
        if (UniformRandom() <= 0.50) AngleChange = NormalRandom(-130, -130 * 0.25);
        NewAngle = (std::int32_t) Camera::GetAngle() + AngleChange;
        if (NewAngle < 0) NewAngle = 364 + AngleChange;


        if (UniformRandom() <= 0.15)
        {
            const auto CurrentPitch = Camera::GetPitch();
            NewPitch = CurrentPitch + NormalRandom(15, 15 * 0.05);
            if (UniformRandom() <= 0.50 && CurrentPitch > (Camera::MIDDLE + 25))
                NewPitch = CurrentPitch + NormalRandom(-15, -15 * 0.05);
        }

        Script::SetStatus("Moving camera a long amount");
        Antiban::Tracker::TimesMovedCamera++;
        Antiban::Tracker::TimesLongCamera++;
        CameraTask::Start( { NewAngle, NewPitch } );
        return true;
    }

    bool DestinationCamera()
    {
        const auto Dest = Minimap::GetDestination();
        const auto Pos = Minimap::GetPosition();
        if (Dest && Pos && Pos.DistanceFrom(Dest) >= 3.00)
        {
            Script::SetStatus("Moving camera towards our current destination");
            Antiban::Tracker::TimesMovedCamera++;
            Antiban::Tracker::TimesDestinationCamera++;
            CameraTask::Start( { -1, -1, Dest } );
            return true;
        }
        return false;
    }

    bool TabOut(bool MouseOff, std::int32_t Time, bool GainBack)
    {
        if (MouseOff)
            Antiban::MouseOffClient(true);
        else
            Antiban::LoseClientFocus();

        Countdown C = Countdown(Time);
        while (C)
        {
            if (Terminate) return false;
            if (Time > 1000)
                Wait(500);
            else
                Wait(50);
        }

        if (GainBack)
            GainFocus();

        Antiban::Tracker::TimesTabbedOut++;
        Antiban::Tracker::TimeSpentTabbedOut += Time;
        Antiban::Tracker::TimeSpentAFK += Time;
        return true;
    }

    bool TabOut_Quick()
    {
        bool MouseOff = UniformRandom() <= 0.65;
        bool GainBack = UniformRandom() <= 0.35;
        std::int32_t Delay = Antiban::GenerateDelayFromPassivity(5000, 15000, 1.6, 0.15);
        Script::SetStatus("Tabbing out for " + MillisToHumanLong(Delay) + ((MouseOff) ? (" by mousing off client") : ""));

        Antiban::Tracker::TimesTabbedOut_Quick++;
        return Antiban::TabOut(MouseOff, Delay, GainBack);
    }

    bool TabOut_Medium()
    {
        bool MouseOff = UniformRandom() <= 0.65;
        bool GainBack = UniformRandom() <= 0.35;
        std::int32_t Delay = Antiban::GenerateDelayFromPassivity(15000, 25000, 1.6, 0.15);
        Script::SetStatus("Tabbing out for " + MillisToHumanLong(Delay) + ((MouseOff) ? (" by mousing off client") : ""));

        Antiban::Tracker::TimesTabbedOut_Medium++;
        return Antiban::TabOut(MouseOff, Delay, GainBack);
    }

    bool TabOut_Long()
    {
        bool MouseOff = UniformRandom() <= 0.65;
        bool GainBack = UniformRandom() <= 0.35;
        std::int32_t Delay = Antiban::GenerateDelayFromPassivity(45000, 95000, 1.4, 0.15);
        Script::SetStatus("Tabbing out for " + MillisToHumanLong(Delay) + ((MouseOff) ? (" by mousing off client") : ""));

        Antiban::Tracker::TimesTabbedOut_Long++;
        return Antiban::TabOut(MouseOff, Delay, GainBack);
    }

    bool AFK(bool TabOut, std::int32_t Time, bool MouseOff, bool GainBack)
    {
        if (TabOut)
        {
            if (Antiban::TabOut(MouseOff, Time, GainBack))
            {
                Antiban::Tracker::TimesAFKed++;
                Antiban::Tracker::TimeSpentAFK += Time;
                return true;
            }
        }
        else
        {
            Countdown C = Countdown(Time);
            while (C)
            {
                if (Terminate) return false;
                if (Time > 1000)
                    Wait(500);
                else
                    Wait(50);
            }

            Antiban::Tracker::TimesAFKed++;
            Antiban::Tracker::TimeSpentAFK += Time;
            return true;
        }
        return false;
    }

    bool AFK_Quick()
    {
        bool TabOut = false;
        enum TENDENCY { TENDENCY_NEVER = 0, TENDENCY_RARELY = 25, TENDENCY_REGULARLY = 50, TENDENCY_FREQUENTLY = 75, TENDENCY_VERY_FREQUENTLY = 100 };
        switch ((TENDENCY) Profile::GetDouble(Profile::Var_TabOut_Tendency) * 100)
        {
            case TENDENCY_NEVER:            TabOut = UniformRandom() <= 0.05; break;
            case TENDENCY_RARELY:           TabOut = UniformRandom() <= 0.10; break;
            case TENDENCY_REGULARLY:        TabOut = UniformRandom() <= 0.40; break;
            case TENDENCY_FREQUENTLY:       TabOut = UniformRandom() <= 0.65; break;
            case TENDENCY_VERY_FREQUENTLY:  TabOut = UniformRandom() <= 0.75; break;
            default: break;
        }
        if (TabOut) return Antiban::TabOut_Quick();

        std::int32_t Delay = Antiban::GenerateDelayFromPassivity(5000, 15000, 1.6, 0.15);
        Script::SetStatus("Going AFK for " + MillisToHumanLong(Delay));

        Antiban::Tracker::TimesAFK_Quick++;
        return Antiban::AFK(false, Delay);
    }

    bool AFK_Medium()
    {
        bool TabOut = false;
        enum TENDENCY { TENDENCY_NEVER = 0, TENDENCY_RARELY = 25, TENDENCY_REGULARLY = 50, TENDENCY_FREQUENTLY = 75, TENDENCY_VERY_FREQUENTLY = 100 };
        switch ((TENDENCY) Profile::GetDouble(Profile::Var_TabOut_Tendency) * 100)
        {
            case TENDENCY_NEVER:            TabOut = UniformRandom() <= 0.05; break;
            case TENDENCY_RARELY:           TabOut = UniformRandom() <= 0.10; break;
            case TENDENCY_REGULARLY:        TabOut = UniformRandom() <= 0.40; break;
            case TENDENCY_FREQUENTLY:       TabOut = UniformRandom() <= 0.65; break;
            case TENDENCY_VERY_FREQUENTLY:  TabOut = UniformRandom() <= 0.75; break;
            default: break;
        }
        if (TabOut) return Antiban::TabOut_Medium();

        std::int32_t Delay = Antiban::GenerateDelayFromPassivity(15000, 25000, 1.6, 0.15);
        Script::SetStatus("Going AFK for " + MillisToHumanLong(Delay));

        Antiban::Tracker::TimesAFK_Medium++;
        return Antiban::AFK(false, Delay);
    }

    bool AFK_Long()
    {
        bool TabOut = false;
        enum TENDENCY { TENDENCY_NEVER = 0, TENDENCY_RARELY = 25, TENDENCY_REGULARLY = 50, TENDENCY_FREQUENTLY = 75, TENDENCY_VERY_FREQUENTLY = 100 };
        switch ((TENDENCY) Profile::GetDouble(Profile::Var_TabOut_Tendency) * 100)
        {
            case TENDENCY_NEVER:            TabOut = UniformRandom() <= 0.05; break;
            case TENDENCY_RARELY:           TabOut = UniformRandom() <= 0.10; break;
            case TENDENCY_REGULARLY:        TabOut = UniformRandom() <= 0.40; break;
            case TENDENCY_FREQUENTLY:       TabOut = UniformRandom() <= 0.65; break;
            case TENDENCY_VERY_FREQUENTLY:  TabOut = UniformRandom() <= 0.80; break;
            default: break;
        }
        if (TabOut) return Antiban::TabOut_Long();

        std::int32_t Delay = Antiban::GenerateDelayFromPassivity(45000, 95000, 1.4, 0.15);
        Script::SetStatus("Going AFK for " + MillisToHumanLong(Delay));

        Antiban::Tracker::TimesAFK_Long++;
        return Antiban::AFK(false, Delay);
    }

    std::int32_t GenerateDelayFromPassivity(std::int32_t Min, std::int32_t Max, double Multiplier, double ChanceToMultiply)
    {
        auto Delay = BinomialRandom(Min, Max, Profile::GetDouble(Profile::Var_Passivity));

        if (Multiplier > 0.00 && UniformRandom() <= ChanceToMultiply)
        {
            DebugLog("Multiplying delay");
            Delay *= Multiplier;
        }

        Delay = NormalRandom(Delay, Delay * 0.12);
        DebugLog("Delay > {}", Delay);
        return Delay;
    }

    void DelayFromPassivity(std::int32_t Min, std::int32_t Max, double Multiplier, double ChanceToMultiply)
    {
        Wait(Antiban::GenerateDelayFromPassivity(Min, Max, Multiplier, ChanceToMultiply));
    }
}