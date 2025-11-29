/**
 * @file McastInterfaces.cpp
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#include "McastInterfaces.h"
#include <etcpal/cpp/netint.h>

namespace sacndeathray {

static const std::unordered_map<QHostAddress::NetworkLayerProtocol, etcpal_iptype_t>
    kProtocolIpTypeMap{
        {QHostAddress::NetworkLayerProtocol::IPv4Protocol, kEtcPalIpTypeV4},
        {QHostAddress::NetworkLayerProtocol::IPv6Protocol, kEtcPalIpTypeV6},
    };

std::vector<SacnMcastInterface> mcastInterfaces(const QNetworkInterface &iface)
{
    std::vector<SacnMcastInterface> r;

    for (const auto &address : iface.addressEntries()) {
        const auto ip = etcpal::IpAddr::FromString(address.ip().toString().toStdString());
        auto sacnNetInterface = etcpal::netint::GetInterfaceWithIp(ip);
        if (!sacnNetInterface.has_value() || !sacnNetInterface->IsValid()
            || !etcpal::netint::IsUp(sacnNetInterface->index())) {
            continue;
        }
        r.emplace_back(SacnMcastInterface{
        .iface = {
            .ip_type = sacnNetInterface->addr().raw_type(),
            .index = sacnNetInterface->index().value(),
        },
        .status = kEtcPalErrOk,
        });
    }

    return r;
}

} // namespace sacndeathray
