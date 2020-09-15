#ifndef TLIB_TSCRIPT_HPP_INCLUDED
#define TLIB_TSCRIPT_HPP_INCLUDED

#include <Core/Script.hpp>
#include <thread>
#include <mutex>
#include <Core/Time.hpp>
#include "../Include/Tools/spdlog/spdlog.h"
#include "../Include/Tools/spdlog/async.h"
#include "../Include/Tools/spdlog/details/log_msg.h"

#define InfoLog(...)        SPDLOG_INFO(__VA_ARGS__)
#define DebugLog(...)       SPDLOG_DEBUG(__VA_ARGS__)
#define WarnLog(...)        SPDLOG_WARN(__VA_ARGS__)
#define ErrorLog(...)       SPDLOG_ERROR(__VA_ARGS__)
#define CriticalLog(...)    SPDLOG_CRITICAL(__VA_ARGS__)

class Script
{
public:
    static void Setup(bool EnableDebug = true);
    static void Start(void (*PainterFunc)() = nullptr, bool StartPaintThread = false);

    inline static const std::string& GetName() { return Name; };
    inline static const std::string& GetVersion() { return Version; };
    inline static const std::string& GetAuthor() { return Author; };

    template<typename... Args>
    inline static void SetStatus(spdlog::level::level_enum lvl, spdlog::string_view_t fmt, const Args &... args)
    {
        std::lock_guard<std::mutex> Lock (Script::StatusLock);
        if (ScriptStatus)
        {
            ScriptStatus->log(lvl, fmt, args...);
            std::string New = fmt::format(fmt, args...);
            if (New != Script::StatusStr)
                DebugLog(New);
            Script::StatusStr = std::move(New);
        }
    }

    template<typename... Args>
    inline static void SetStatus(spdlog::string_view_t fmt, const Args &... args) { Script::SetStatus(spdlog::level::info, fmt, args...); };

    static std::string GetStatus();

    static std::int64_t GetTimeElapsed();
    static void PauseTimer();
    static void ResumeTimer();
    static void RestartTimer();

    template<typename... Args>
    inline static void Stop(spdlog::string_view_t fmt, const Args &... args)
    {
        Script::SetStatus(spdlog::level::critical, fmt, args...);
        TerminateScript();
    }

    static void Shutdown();

    class Tools
    {
    public:
        static std::string FormatCommas(std::int64_t Int);
        static std::string FormatRunescapeGold(std::int64_t Gold);
        static std::int64_t ToHour(std::int64_t Start, std::int64_t Current);
    };

private:
    inline static std::string StatusStr;
    inline static std::mutex StatusLock;

    inline static std::string Name;
    inline static std::string Version;
    inline static std::string Category;
    inline static std::string Author;

    inline static Timer ScriptTimer = Timer(false);
    inline static std::mutex TimerLock;

    static void (*PaintFunc)();
    static void StartPaint();
    static void Paint();
    static void StopPaint();

    inline static std::atomic<bool> PaintThreadTerm = false;
    inline static std::thread* PaintThread = nullptr;

    inline static std::shared_ptr<spdlog::logger> ScriptStatus= nullptr;
    inline static std::shared_ptr<spdlog::async_logger> ScriptLogger = nullptr;

public: // Logging

    template<typename... Args>
    inline static void Log(spdlog::source_loc source, spdlog::level::level_enum lvl, spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->log(source, lvl, fmt, args...);
    }

    template<typename... Args>
    inline static void Log(spdlog::level::level_enum lvl, spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->log(spdlog::source_loc{}, lvl, fmt, args...);
    }

    template<typename... Args>
    inline static void Trace(spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->trace(fmt, args...);
    }

    template<typename... Args>
    inline static void Debug(spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->debug(fmt, args...);
    }

    template<typename... Args>
    inline static void Info(spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->info(fmt, args...);
    }

    template<typename... Args>
    inline static void Warn(spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->warn(fmt, args...);
    }

    template<typename... Args>
    inline static void Error(spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->error(fmt, args...);
    }

    template<typename... Args>
    inline static void Critical(spdlog::string_view_t fmt, const Args &... args)
    {
        if (ScriptLogger) ScriptLogger->critical(fmt, args...);
    }

    template<typename T>
    inline static void Log(spdlog::source_loc source, spdlog::level::level_enum lvl, const T &msg)
    {
        if (ScriptLogger) ScriptLogger->log(source, lvl, msg);
    }

    template<typename T>
    inline static void Log(spdlog::level::level_enum lvl, const T &msg)
    {
        if (ScriptLogger) ScriptLogger->log(lvl, msg);
    }

    template<typename T>
    inline static void Trace(const T &msg)
    {
        if (ScriptLogger) ScriptLogger->trace(msg);
    }

    template<typename T>
    inline static void Debug(const T &msg)
    {
        if (ScriptLogger) ScriptLogger->debug(msg);
    }

    template<typename T>
    inline static void Info(const T &msg)
    {
        if (ScriptLogger) ScriptLogger->info(msg);
    }

    template<typename T>
    inline static void Warn(const T &msg)
    {
        if (ScriptLogger) ScriptLogger->warn(msg);
    }

    template<typename T>
    inline static void Error(const T &msg)
    {
        if (ScriptLogger) ScriptLogger->error(msg);
    }

    template<typename T>
    inline static void Critical(const T &msg)
    {
        if (ScriptLogger) ScriptLogger->critical(msg);
    }
};

#endif // TLIB_TSCRIPT_HPP_INCLUDED