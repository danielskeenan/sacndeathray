/**
 * @file NetInt.cpp
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#include "NetInt.h"
#include <QCoreApplication>

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

void printNetInts(std::ostream &out)
{
    const auto netInts = getNetInts();
    if (netInts.empty()) {
        out << qPrintable(qApp->translate("main", "No usable network interfaces."))
                  << std::endl;
        return;
    }

    for (const auto &netInt : netInts) {
        out << qPrintable(qApp->translate("main", "%1: %2 (%3)")
                                    .arg(netInt.index())
                                    .arg(netInt.name())
                                    .arg(netInt.humanReadableName()))
                  << std::endl
                  << "  " << qPrintable(netInt.hardwareAddress()) << std::endl;
        for (const auto &address : netInt.addressEntries()) {
            out << "  - " << qPrintable(address.ip().toString()) << std::endl;
        }
        out << std::endl;
    }
}

} // namespace sacndeathray
