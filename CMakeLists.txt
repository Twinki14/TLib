cmake_minimum_required(VERSION 3.16)
project(TLib) # Change this to your projects/Scripts name

# C++ flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Os -s -m32")
set(CMAKE_SHARED_LINKER_FLAGS "-Os -s -m32 -static-libgcc -static-libstdc++ -static")

file(GLOB_RECURSE   SOURCES     RELATIVE ${CMAKE_SOURCE_DIR} "Source/*.cpp" "Source/*.c")
file(GLOB_RECURSE   HEADERS     RELATIVE ${CMAKE_SOURCE_DIR} "Include/*.hpp" "Include/*.h")

message("-------")
foreach(S IN LISTS SOURCES HEADERS)
    message("${S}")
endforeach()
message("-------")

# Select all source files
set(SOURCE_FILES
        Source/TScript.cpp
        Source/Tools/OSRSBox/Items.cpp
        Source/Tools/OSRSBox/OSRSBox.cpp
        Source/Tools/Prices.cpp
        Source/Tools/Tools.cpp
        Source/Tools/WinnHttpComm.cpp
        Source/Tools/json11.cpp
        Source/Tools/sqlite/sqlite3.c
        Source/Utilities/Antiban.cpp
        Source/Utilities/BackgroundTask/AuthenticateTask.cpp
        Source/Utilities/BackgroundTask/BackgroundTask.cpp
        Source/Utilities/BackgroundTask/CameraTask.cpp
        Source/Utilities/Bank.cpp
        Source/Utilities/Boosts.cpp
        Source/Utilities/Buffs.cpp
        Source/Utilities/Containers.cpp
        Source/Utilities/Extensions.cpp
        Source/Utilities/Food.cpp
        Source/Utilities/GearSets.cpp
        Source/Utilities/Inventory.cpp
        Source/Utilities/Mainscreen.cpp
        Source/Utilities/Poh.cpp
        Source/Utilities/Prayer.cpp
        Source/Utilities/RunePouch.cpp
        Source/Utilities/Runes.cpp
        Source/Utilities/StatTracker.cpp
        Source/Utilities/TProfile.cpp
        Include/TScript.hpp
        Include/Tools/OSRSBox/Items.hpp
        Include/Tools/OSRSBox/OSRSBox.hpp
        Include/Tools/OSRSBox/schemas/Item.hpp
        Include/Tools/PaintBitmap.hpp
        Include/Tools/Prices.hpp
        Include/Tools/TGotify.hpp
        Include/Tools/Tools.hpp
        Include/Tools/WinnHttpComm.hpp
        Include/Tools/configuru.hpp
        Include/Tools/cute_files.h
        Include/Tools/json.hpp
        Include/Tools/json11.hpp
        Include/Tools/spdlog/async.h
        Include/Tools/spdlog/async_logger-inl.h
        Include/Tools/spdlog/async_logger.h
        Include/Tools/spdlog/cfg/argv.h
        Include/Tools/spdlog/cfg/env.h
        Include/Tools/spdlog/cfg/helpers-inl.h
        Include/Tools/spdlog/cfg/helpers.h
        Include/Tools/spdlog/cfg/log_levels.h
        Include/Tools/spdlog/common-inl.h
        Include/Tools/spdlog/common.h
        Include/Tools/spdlog/details/backtracer-inl.h
        Include/Tools/spdlog/details/backtracer.h
        Include/Tools/spdlog/details/circular_q.h
        Include/Tools/spdlog/details/console_globals.h
        Include/Tools/spdlog/details/file_helper-inl.h
        Include/Tools/spdlog/details/file_helper.h
        Include/Tools/spdlog/details/fmt_helper.h
        Include/Tools/spdlog/details/log_msg-inl.h
        Include/Tools/spdlog/details/log_msg.h
        Include/Tools/spdlog/details/log_msg_buffer-inl.h
        Include/Tools/spdlog/details/log_msg_buffer.h
        Include/Tools/spdlog/details/mpmc_blocking_q.h
        Include/Tools/spdlog/details/null_mutex.h
        Include/Tools/spdlog/details/os-inl.h
        Include/Tools/spdlog/details/os.h
        Include/Tools/spdlog/details/pattern_formatter-inl.h
        Include/Tools/spdlog/details/pattern_formatter.h
        Include/Tools/spdlog/details/periodic_worker-inl.h
        Include/Tools/spdlog/details/periodic_worker.h
        Include/Tools/spdlog/details/registry-inl.h
        Include/Tools/spdlog/details/registry.h
        Include/Tools/spdlog/details/synchronous_factory.h
        Include/Tools/spdlog/details/tcp_client-windows.h
        Include/Tools/spdlog/details/tcp_client.h
        Include/Tools/spdlog/details/thread_pool-inl.h
        Include/Tools/spdlog/details/thread_pool.h
        Include/Tools/spdlog/details/windows_include.h
        Include/Tools/spdlog/fmt/bin_to_hex.h
        Include/Tools/spdlog/fmt/bundled/chrono.h
        Include/Tools/spdlog/fmt/bundled/color.h
        Include/Tools/spdlog/fmt/bundled/compile.h
        Include/Tools/spdlog/fmt/bundled/core.h
        Include/Tools/spdlog/fmt/bundled/format-inl.h
        Include/Tools/spdlog/fmt/bundled/format.h
        Include/Tools/spdlog/fmt/bundled/locale.h
        Include/Tools/spdlog/fmt/bundled/ostream.h
        Include/Tools/spdlog/fmt/bundled/posix.h
        Include/Tools/spdlog/fmt/bundled/printf.h
        Include/Tools/spdlog/fmt/bundled/ranges.h
        Include/Tools/spdlog/fmt/fmt.h
        Include/Tools/spdlog/fmt/ostr.h
        Include/Tools/spdlog/formatter.h
        Include/Tools/spdlog/fwd.h
        Include/Tools/spdlog/logger-inl.h
        Include/Tools/spdlog/logger.h
        Include/Tools/spdlog/pattern_formatter-inl.h
        Include/Tools/spdlog/pattern_formatter.h
        Include/Tools/spdlog/sinks/android_sink.h
        Include/Tools/spdlog/sinks/ansicolor_sink-inl.h
        Include/Tools/spdlog/sinks/ansicolor_sink.h
        Include/Tools/spdlog/sinks/base_sink-inl.h
        Include/Tools/spdlog/sinks/base_sink.h
        Include/Tools/spdlog/sinks/basic_file_sink-inl.h
        Include/Tools/spdlog/sinks/basic_file_sink.h
        Include/Tools/spdlog/sinks/daily_file_sink.h
        Include/Tools/spdlog/sinks/dist_sink.h
        Include/Tools/spdlog/sinks/dup_filter_sink.h
        Include/Tools/spdlog/sinks/msvc_sink.h
        Include/Tools/spdlog/sinks/null_sink.h
        Include/Tools/spdlog/sinks/ostream_sink.h
        Include/Tools/spdlog/sinks/ringbuffer_sink.h
        Include/Tools/spdlog/sinks/rotating_file_sink-inl.h
        Include/Tools/spdlog/sinks/rotating_file_sink.h
        Include/Tools/spdlog/sinks/sink-inl.h
        Include/Tools/spdlog/sinks/sink.h
        Include/Tools/spdlog/sinks/stdcout_wincolor_sink-inl.h
        Include/Tools/spdlog/sinks/stdcout_wincolor_sink.h
        Include/Tools/spdlog/sinks/stdout_color_sinks-inl.h
        Include/Tools/spdlog/sinks/stdout_color_sinks.h
        Include/Tools/spdlog/sinks/stdout_sinks-inl.h
        Include/Tools/spdlog/sinks/stdout_sinks.h
        Include/Tools/spdlog/sinks/syslog_sink.h
        Include/Tools/spdlog/sinks/systemd_sink.h
        Include/Tools/spdlog/sinks/tcp_sink.h
        Include/Tools/spdlog/sinks/win_eventlog_sink.h
        Include/Tools/spdlog/sinks/wincolor_sink-inl.h
        Include/Tools/spdlog/sinks/wincolor_sink.h
        Include/Tools/spdlog/spdlog-inl.h
        Include/Tools/spdlog/spdlog.h
        Include/Tools/spdlog/tweakme.h
        Include/Tools/spdlog/version.h
        Include/Tools/sqlite/sqlite3.h
        Include/Tools/sqlite/sqlite3ext.h
        Include/Tools/sqlite/sqlite_orm.h
        Include/Utilities/Antiban.hpp
        Include/Utilities/BackgroundTask/AuthenticateTask.hpp
        Include/Utilities/BackgroundTask/BackgroundTask.hpp
        Include/Utilities/BackgroundTask/CameraTask.hpp
        Include/Utilities/Bank.hpp
        Include/Utilities/Boosts.hpp
        Include/Utilities/Buffs.hpp
        Include/Utilities/Containers.hpp
        Include/Utilities/Extensions.hpp
        Include/Utilities/Food.hpp
        Include/Utilities/GearSets.hpp
        Include/Utilities/Inventory.hpp
        Include/Utilities/Mainscreen.hpp
        Include/Utilities/Poh.hpp
        Include/Utilities/Prayer.hpp
        Include/Utilities/RunePouch.hpp
        Include/Utilities/Runes.hpp
        Include/Utilities/StatTracker.hpp
        Include/Utilities/TProfile.hpp)

# Set directories
set(ALPACABOT_DIRECTORY $ENV{HOMEDRIVE}$ENV{HOMEPATH}\\AlpacaBot)
message("AlpacaBot directory > " ${ALPACABOT_DIRECTORY})

add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_LONGHORN)

# Build the library
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Os -s -m32" LINK_FLAGS "-Os -s -m32")

# Include and Library directories
include_directories(Libs)
include_directories(Include/Tools)
include_directories(${ALPACABOT_DIRECTORY}\\Include)
target_link_directories(${PROJECT_NAME} PRIVATE ${ALPACABOT_DIRECTORY}/Library)

# Link libs
target_link_libraries(${PROJECT_NAME} AlpacaLibrary)
target_link_libraries(${PROJECT_NAME} winhttp)
target_link_libraries(${PROJECT_NAME} Ws2_32)