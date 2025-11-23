/**
 * @file Runner.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "Runner.h"
#include <spdlog/spdlog.h>

namespace sacndeathray {

void Runner::start()
{
    SPDLOG_INFO("Setting up transmitter");
    // TODO: Implement.

    Q_EMIT(finished());
}

} // namespace sacndeathray
