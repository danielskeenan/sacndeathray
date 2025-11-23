/**
 * @file ProgramOptions.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERCLI_PROGRAMOPTIONS_H
#define SACNDEATHRAY_TRANSMITTERCLI_PROGRAMOPTIONS_H

#include "sacndeathray/config.h"
#include <spdlog/common.h>
#include <vector>
#include <QHostAddress>

namespace sacndeathray {

struct ProgramOptions
{
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::info;
    std::vector<uint16_t> universes{1};
    double rate = 22;
    uint16_t port = config::kMessagePort;
    QHostAddress receiverAddress;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERCLI_PROGRAMOPTIONS_H
