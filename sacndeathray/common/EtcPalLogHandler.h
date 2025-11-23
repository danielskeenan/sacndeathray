/**
 * @file EtcPalLogHandler.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_ETCPALLOGHANDLER_H
#define SACNDEATHRAY_COMMON_ETCPALLOGHANDLER_H

#include <etcpal/cpp/log.h>

namespace sacndeathray {

/**
 * Sends EtcPal log messages to spdlog.
 */
class EtcPalLogHandler : public etcpal::LogMessageHandler
{
public:
    etcpal::LogTimestamp GetLogTimestamp() override;
    void HandleLogMessage(const EtcPalLogStrings &strings) override;
};

/**
 * Deleter for etcpal::Logger objects.
 */
struct EtcPalLoggerDeleter
{
    void operator()(etcpal::Logger *logger) const
    {
        logger->Shutdown();
        delete logger;
    }
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_COMMON_ETCPALLOGHANDLER_H
