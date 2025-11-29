/**
 * @file ReceiverOptions.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERLIB_RECEIVEROPTIONS_H
#define SACNDEATHRAY_RECEIVERLIB_RECEIVEROPTIONS_H

#include <cstdint>
#include <QNetworkInterface>

namespace sacndeathray {

struct ReceiverOptions
{
    QNetworkInterface netInt;
    uint16_t port;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_RECEIVERLIB_RECEIVEROPTIONS_H
