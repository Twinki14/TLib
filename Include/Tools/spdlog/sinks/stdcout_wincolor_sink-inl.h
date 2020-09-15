// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include "ostream_ansicolor_sink.h"
#endif

#include <spdlog/details/console_globals.h>
#include <spdlog/pattern_formatter.h>

namespace spdlog {
namespace sinks {

template<typename ConsoleMutex>
SPDLOG_INLINE stdcout_wincolor_sink<ConsoleMutex>::stdcout_wincolor_sink(color_mode mode, bool force_flush)
    : ostream_(std::cout)
    , mutex_(ConsoleMutex::mutex())
    , formatter_(details::make_unique<spdlog::pattern_formatter>())
{
    set_color_mode(mode);
    colors_[level::trace] = WHITE;
    colors_[level::debug] = CYAN;
    colors_[level::info] = GREEN;
    colors_[level::warn] = YELLOW | BOLD;
    colors_[level::err] = RED | BOLD;                         // red bold
    colors_[level::critical] = BACKGROUND_RED | WHITE | BOLD; // white bold on red background
    colors_[level::off] = 0;
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::set_color(level::level_enum color_level, WORD color)
{
    std::lock_guard<mutex_t> lock(mutex_);
    colors_[color_level] = color;
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::log(const details::log_msg &msg)
{
    // Wrap the originally formatted message in color codes.
    // If color is not supported in the terminal, log as is instead.
    std::lock_guard<mutex_t> lock(mutex_);

    memory_buf_t formatted;
    formatter_->format(msg, formatted);
    if (should_do_colors_ && msg.color_range_end > msg.color_range_start)
    {
        // before color range
        print_range_(formatted, 0, msg.color_range_start);

        // in color range
        auto orig_attribs = set_foreground_color_(colors_[msg.level]);
        print_range_(formatted, msg.color_range_start, msg.color_range_end);
        // reset to orig colors
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hStdout, orig_attribs);
        print_range_(formatted, msg.color_range_end, formatted.size());
    }
    else // no color
    {
        ostream_.write(formatted.data(), static_cast<std::streamsize>(formatted.size()));
        //print_range_(formatted, 0, formatted.size());
    }
    //
    ostream_.flush();
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::flush()
{
    std::lock_guard<mutex_t> lock(mutex_);
    ostream_.flush();
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::set_pattern(const std::string &pattern)
{
    std::lock_guard<mutex_t> lock(mutex_);
    formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
{
    std::lock_guard<mutex_t> lock(mutex_);
    formatter_ = std::move(sink_formatter);
}

template<typename ConsoleMutex>
SPDLOG_INLINE bool stdcout_wincolor_sink<ConsoleMutex>::should_color()
{
    return should_do_colors_;
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::set_color_mode(color_mode mode)
{
    switch (mode)
    {
    case color_mode::always:
        should_do_colors_ = true;
        return;
    case color_mode::automatic:
        should_do_colors_ = details::os::is_color_terminal();
        return;
    case color_mode::never:
        should_do_colors_ = false;
        return;
    }
}

// set foreground color and return the orig console attributes (for resetting later)
template<typename ConsoleMutex>
WORD SPDLOG_INLINE stdcout_wincolor_sink<ConsoleMutex>::set_foreground_color_(WORD attribs)
{
    CONSOLE_SCREEN_BUFFER_INFO orig_buffer_info;

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &orig_buffer_info);
    WORD back_color = orig_buffer_info.wAttributes;
    // retrieve the current background color
    back_color &= static_cast<WORD>(~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY));
    // keep the background color unchanged
    SetConsoleTextAttribute(hStdout, attribs | back_color);
    return orig_buffer_info.wAttributes; // return orig attribs
}

template<typename ConsoleMutex>
SPDLOG_INLINE void stdcout_wincolor_sink<ConsoleMutex>::print_range_(const memory_buf_t &formatted, size_t start, size_t end)
{
    ostream_.write(formatted.data() + start, end - start);
}

} // namespace sinks
} // namespace spdlog
