#include <iomanip>
#include <Game/Tools/Profile.hpp>
#include "../Include/TScript.hpp"
#include "../Include/Tools/spdlog/sinks/dup_filter_sink.h"
#include "../Include/Tools/spdlog/sinks/stdcout_wincolor_sink.h"
#include "../Include/Tools/spdlog/sinks/rotating_file_sink.h"

#define CUTE_FILES_IMPLEMENTATION
#include "../Include/Tools/cute_files.h"

void (*Script::PaintFunc)() = nullptr;

void LogPath_Visit(cf_file_t* file, void* udata)
{
    if (cf_match_ext(file, ".txt") && std::string(file->name).find("Log.") != std::string::npos)
        ((std::vector<cf_file_t>*) udata)->emplace_back(*file);
}

void Script::Setup(bool EnableDebug)
{
    ScriptInfo Info;
    GetScriptInfo(Info);

    Name = Info.Name;
    Version = Info.Version;
    Category = Info.Category;
    Author = Info.Author;

    // ScriptStatus
    auto ScriptStatus_Filter = std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(10));
    ScriptStatus_Filter->add_sink(std::make_shared<spdlog::sinks::stdcout_wincolor_sink_st>(spdlog::color_mode::automatic));
    ScriptStatus_Filter->set_pattern("[%r] [" + Name +"] %^Status%$ > %v");

    //const std::string ProfileName = Profile::GetUsername();
    //const std::string LogFileName = "Log" + std::string(!ProfileName.empty() ? ("_" + ProfileName) : "") + ".txt";

    const std::string ScriptPath = GetScriptPath();
    const std::string LogPath = ScriptPath + "\\Logs";
    const std::string LogFileName = "Log." + std::to_string(GetCurrentProcessId()) + ".txt";

    // Delete the earliest log files if there's more than 10
    if (!ScriptPath.empty() && !Name.empty() && ScriptPath.find(Name) != std::string::npos && cf_file_exists(LogPath.c_str()))
    {
        std::vector<cf_file_t> LogFiles;
        cf_traverse(LogPath.c_str(), LogPath_Visit, &LogFiles);

        std::int32_t Overflow = LogFiles.size() - 10;
        std::cout << "LogFilesOverflow > " << Overflow << std::endl;
        for (std::int32_t I = 0; I < Overflow; I++)
        {
            if (LogFiles.empty())
                continue;

            auto Earliest = LogFiles.begin();
            for (auto File = LogFiles.begin(); File < LogFiles.end(); File++)
                if (cf_compare_file_times_by_path(File->path, Earliest->path) == -1)
                    Earliest = File;

            if (cf_match_ext(Earliest.base(), ".txt") && std::string(Earliest->name).find("Log.") != std::string::npos)
            {
                std::cout << "Removing > " << Earliest->path << std::endl;
                if (std::remove(Earliest->path) == 0)
                    LogFiles.erase(Earliest);
            }
        }
    }

    // ScriptLogger
    auto ScriptFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(LogPath + "\\" + LogFileName, 1048576 * 10, 2, true);
    ScriptFileSink->set_pattern("[%D] [%r] [%l] > %! > %v");
    ScriptFileSink->set_level(spdlog::level::trace);

    auto ScriptSink = std::make_shared<spdlog::sinks::stdcout_wincolor_sink_mt>();
    ScriptSink->set_pattern("[%r] [%^%l%$] > %! > %v");
    ScriptSink->set_level(EnableDebug ? spdlog::level::trace : spdlog::level::info);

    auto ScriptLogger_DistSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
    ScriptLogger_DistSink->add_sink(ScriptSink);
    ScriptLogger_DistSink->add_sink(ScriptFileSink);

    spdlog::init_thread_pool(128, 1);
    ScriptStatus = std::make_shared<spdlog::async_logger>("ScriptStatus", ScriptStatus_Filter, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
    ScriptLogger = std::make_shared<spdlog::async_logger>("ScriptLogger", ScriptLogger_DistSink, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
    spdlog::set_default_logger(ScriptLogger);

    spdlog::set_level(spdlog::level::trace);
}

void Script::Start(void (*PainterFunc)(), bool StartPaintThread)
{
    Script::ResumeTimer();
    Script::RestartTimer();
    Script::PaintFunc = PainterFunc;
    if (StartPaintThread) Script::StartPaint();
    Script::SetStatus("Starting");
}

std::string Script::GetStatus()
{
    std::lock_guard<std::mutex> Lock (Script::StatusLock);
    return Script::StatusStr;
}

std::int64_t Script::GetTimeElapsed()
{
    std::lock_guard<std::mutex> Lock (Script::TimerLock);
    return Script::ScriptTimer.GetTimeElapsed();
}

void Script::PauseTimer()
{
    std::lock_guard<std::mutex> Lock (Script::TimerLock);
    Script::ScriptTimer.Suspend();
}

void Script::ResumeTimer()
{
    std::lock_guard<std::mutex> Lock (Script::TimerLock);
    Script::ScriptTimer.Resume();
}

void Script::RestartTimer()
{
    std::lock_guard<std::mutex> Lock (Script::TimerLock);
    Script::ScriptTimer.Restart();
}

void Script::Shutdown()
{
    spdlog::shutdown();
    Script::StopPaint();
}

std::string Script::Tools::FormatCommas(std::int64_t Int)
{
    static bool Set = false;
    if (!Set)
    {
        Set = true;
        std::setlocale(LC_NUMERIC, "");
    }

    char Buffer[128];
    sprintf(Buffer, "%'d", Int);
    return std::string(Buffer);
}

std::string Script::Tools::FormatRunescapeGold(std::int64_t Gold)
{
    if (Gold <= 0) return "0";

    double Millions = (double) Gold / 1000000.0;
    double Thousands = (double) Gold / 1000.0;

    std::stringstream Stream;
    if (Millions >= 1.00)
        Stream << std::fixed << std::setprecision(1) << Millions << "M";
    else if (Thousands >= 1.00)
        Stream << std::fixed << std::setprecision(0) << Thousands << "K";
    else
        Stream << Gold;

    return Stream.str();
}

std::int64_t Script::Tools::ToHour(std::int64_t Start, std::int64_t Current)
{
    std::int64_t TimeElapsed = Script::GetTimeElapsed();
    if (TimeElapsed <= 0) return 0;
    return (((float) (Current - Start) ) / ((float) TimeElapsed / 3600000.0f));
}

void Script::StartPaint()
{
    if (!Script::PaintThread)
    {
        Script::PaintThreadTerm = false;
        Script::PaintThread = new std::thread(Script::Paint);
    }
}

void Script::Paint()
{
    if (!Script::PaintFunc) return;
    while (!Script::PaintThreadTerm && !Terminate)
        Script::PaintFunc();
}

void Script::StopPaint()
{
    if (Script::PaintThread)
    {
        Script::PaintThreadTerm = true;
        if (Script::PaintThread->joinable())
            Script::PaintThread->join();
        delete Script::PaintThread;
        Script::PaintThread = nullptr;
        Script::PaintThreadTerm = false;
    }
}
