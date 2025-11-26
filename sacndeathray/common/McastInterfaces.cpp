/**
 * @file McastInterfaces.cpp
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#include "McastInterfaces.h"
#include <unordered_set>

namespace sacndeathray {

static const std::unordered_map<QHostAddress::NetworkLayerProtocol, etcpal_iptype_t>
    kProtocolIpTypeMap{
        {QHostAddress::NetworkLayerProtocol::IPv4Protocol, kEtcPalIpTypeV4},
        {QHostAddress::NetworkLayerProtocol::IPv6Protocol, kEtcPalIpTypeV6},
    };

std::vector<SacnMcastInterface> mcastInterfaces(const QNetworkInterface &iface)
{
    std::unordered_set<QHostAddress::NetworkLayerProtocol> protocolsUsed;
    for (const auto &addressEntry : iface.addressEntries()) {
        const auto protocol = addressEntry.ip().protocol();
        if (!protocolsUsed.contains(protocol)) {
            protocolsUsed.insert(protocol);
        }
    }

    std::vector<SacnMcastInterface> mcastInterfaces;
    for (const auto protocol : protocolsUsed) {
        const auto ipType = kProtocolIpTypeMap.find(protocol);
        if (ipType != kProtocolIpTypeMap.end()) {
            EtcPalMcastNetintId netint{ipType->second, static_cast<unsigned int>(iface.index())};
            mcastInterfaces.emplace_back(netint);
        }
    }

    return mcastInterfaces;
}

} // namespace sacndeathray
