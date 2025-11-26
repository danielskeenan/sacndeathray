/**
 * @file NetInt.cpp
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#include "NetInt.h"

namespace sacndeathray {

std::vector<QNetworkInterface> getNetInts()
{
    std::vector<QNetworkInterface> r;

    for (const auto &netint : QNetworkInterface::allInterfaces()) {
        if (netint.flags().testFlags(QNetworkInterface::IsRunning | QNetworkInterface::CanMulticast)
            && netint.type() == QNetworkInterface::Ethernet) {
            r.emplace_back(netint);
        }
    }
    return r;
}

} // namespace sacndeathray
