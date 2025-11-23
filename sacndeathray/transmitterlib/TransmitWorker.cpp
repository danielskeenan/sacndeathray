/**
 * @file TransmitWorker.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitWorker.h"
#include "sacndeathray/config.h"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <QTimer>

namespace sacndeathray {

TransmitWorker::TransmitWorker(const Config &config, QObject *parent) :
    QObject(parent), config_(config), timer_(new QTimer(this))
{
    connect(timer_, &QTimer::timeout, this, &TransmitWorker::tick);
}

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

void TransmitWorker::start()
{
    SPDLOG_INFO("Staring transmitter {} on universes {}", config_.cid.ToString(), config_.universes);
    sacn::Source::Settings sourceSettings(config_.cid, config::kProjectDisplayName);
    sourceSettings.manually_process_source = true;
    source_.reset(new sacn::Source);
    auto result = source_->Startup(sourceSettings);
    if (!result.IsOk()) {
        SPDLOG_ERROR("Error starting transmitter: {}", result.ToString());
        throw std::runtime_error("Error starting transmitter.");
    }

    auto netints = mcastInterfaces(config_.iface);
    for (const auto univ : config_.universes) {
        sacn::Source::UniverseSettings univSettings(univ);
        // Try to prevent this test data from showing on real devices.
        univSettings.priority = 0;
        univSettings.send_preview = true;
        source_->AddUniverse(univSettings, netints);
    }

    levelBuffer_.fill(0);
    timer_->start((1 / config_.rate) * 1000);
}

void TransmitWorker::stop()
{
    timer_->stop();
    source_.reset();
}

void TransmitWorker::tick()
{
    source_->ProcessManual(sacn::Source::TickMode::kProcessLevelsOnly);
    // Overflow wraps value back to 0.
    ++levelBuffer_[0];
}

} // namespace sacndeathray
