/**
 * @file ProgramOptions.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERCLI_PROGRAMOPTIONS_H
#define SACNDEATHRAY_RECEIVERCLI_PROGRAMOPTIONS_H

#include "sacndeathray/receiverlib/ReceiverOptions.h"
#include <spdlog/common.h>

namespace sacndeathray {

struct ProgramOptions : public ReceiverOptions
{
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::info;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_RECEIVERCLI_PROGRAMOPTIONS_H
