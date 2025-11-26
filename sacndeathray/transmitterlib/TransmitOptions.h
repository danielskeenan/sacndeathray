/**
 * @file TransmitOptions.h
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERLIB_TRANSMITOPTIONS_H
#define SACNDEATHRAY_TRANSMITTERLIB_TRANSMITOPTIONS_H

#include <cstdint>
#include <vector>
#include <QHostAddress>
#include <QNetworkInterface>

namespace sacndeathray {

struct TransmitOptions
{
    std::vector<uint16_t> universes;
    double rate;
    uint16_t port;
    QHostAddress receiverAddress;
    /** If not set, will autoselect an interface based on the receiver address. */
    std::optional<QNetworkInterface> netInt;
};

}

#endif //SACNDEATHRAY_TRANSMITTERLIB_TRANSMITOPTIONS_H
