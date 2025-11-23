/**
 * @file EtcPalLogHandler.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "EtcPalLogHandler.h"
#include <chrono>
#include <spdlog/spdlog.h>

namespace sacndeathray {
etcpal::LogTimestamp EtcPalLogHandler::GetLogTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto tz = std::chrono::current_zone();
    const auto tzOffset = std::chrono::duration_cast<std::chrono::minutes>(tz->get_info(now).offset);
    const auto days = std::chrono::floor<std::chrono::days>(now);
    const std::chrono::year_month_day date(days);
    const std::chrono::hh_mm_ss timestamp(now - days);
    return etcpal::LogTimestamp(
        static_cast<int>(date.year()),
        static_cast<unsigned int>(date.month()),
        static_cast<unsigned int>(date.day()),
        static_cast<unsigned int>(timestamp.hours().count()),
        static_cast<unsigned int>(timestamp.minutes().count()),
        static_cast<unsigned int>(timestamp.seconds().count()),
        0,
        tzOffset.count());
}

void EtcPalLogHandler::HandleLogMessage(const EtcPalLogStrings &strings)
{
    // Ensure there is something to log.
    if (strings.raw == nullptr) {
        SPDLOG_ERROR("!!!UNDEFINED LOG MESSAGE!!!");
        return;
    }

    // Convert ETC's log levels to spdlog's.
    const spdlog::level::level_enum level = [&strings]() {
        switch (strings.priority) {
        case ETCPAL_LOG_EMERG:
        case ETCPAL_LOG_ALERT:
        case ETCPAL_LOG_CRIT:
            return spdlog::level::critical;
        case ETCPAL_LOG_ERR:
            return spdlog::level::err;
        case ETCPAL_LOG_WARNING:
            return spdlog::level::warn;
        case ETCPAL_LOG_NOTICE:
        case ETCPAL_LOG_INFO:
            return spdlog::level::info;
        case ETCPAL_LOG_DEBUG:
            return spdlog::level::debug;
        }
        // Unhandled log levels are an error.
        return spdlog::level::err;
    }();

    // Do the thing.
    spdlog::log(level, strings.raw);
}
} // namespace sacndeathray
