/**
 * @file ProgramOptions.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERCLI_PROGRAMOPTIONS_H
#define SACNDEATHRAY_TRANSMITTERCLI_PROGRAMOPTIONS_H

#include "sacndeathray/transmitterlib/TransmitOptions.h"
#include <spdlog/common.h>

namespace sacndeathray {

struct ProgramOptions : public TransmitOptions
{
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::info;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERCLI_PROGRAMOPTIONS_H
