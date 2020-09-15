#pragma once

#include <spdlog/details/console_globals.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/sink.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace spdlog {
namespace sinks {

/**
 * This sink prefixes the output with an ANSI escape sequence color code
 * depending on the severity
 * of the message.
 * If no color terminal detected, omit the escape codes.
 */

template<typename ConsoleMutex>
class stdcout_wincolor_sink : public sink
{
public:
    const WORD BOLD = FOREGROUND_INTENSITY;
    const WORD RED = FOREGROUND_RED;
    const WORD GREEN = FOREGROUND_GREEN;
    const WORD CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
    const WORD WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const WORD YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;

    using mutex_t = typename ConsoleMutex::mutex_t;
    stdcout_wincolor_sink(color_mode mode = spdlog::color_mode::automatic, bool force_flush = false);
    ~stdcout_wincolor_sink() override = default;

    stdcout_wincolor_sink(const stdcout_wincolor_sink &other) = delete;
    stdcout_wincolor_sink &operator=(const stdcout_wincolor_sink &other) = delete;
    void set_color(level::level_enum color_level, WORD color);
    void set_color_mode(color_mode mode);
    bool should_color();

    void log(const details::log_msg &msg) override;
    void flush() override;
    void set_pattern(const std::string &pattern) final;
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

protected:
    std::ostream &ostream_;
    bool force_flush_;
    mutex_t &mutex_;
    bool should_do_colors_;
    std::unique_ptr<spdlog::formatter> formatter_;
    std::unordered_map<level::level_enum, WORD, level::level_hasher> colors_;
    // set foreground color and return the orig console attributes (for resetting later)
    WORD set_foreground_color_(WORD attribs);
    void print_range_(const memory_buf_t &formatted, size_t start, size_t end);
};

using stdcout_wincolor_sink_mt = stdcout_wincolor_sink<details::console_mutex>;
using stdcout_wincolor_sink_st = stdcout_wincolor_sink<details::console_nullmutex>;

} // namespace sinks
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
#include "stdcout_wincolor_sink-inl.h"
#endif