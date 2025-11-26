/**
 * @file ReceiverWorker.cpp
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#include "ReceiverWorker.h"
#include "sacndeathray/common/McastInterfaces.h"
#include <spdlog/spdlog.h>
#include <fmt/ranges.h>

namespace sacndeathray {

namespace detail {

void ReceiverHandler::HandleUniverseData(
    sacn::Receiver::Handle receiverHandle,
    const etcpal::SockAddr &sourceAddr,
    const SacnRemoteSource &sourceInfo,
    const SacnRecvUniverseData &universeData)
{
    if (sourceInfo.cid != cid_) {
        // Don't care about this source.
        return;
    }

    const auto checkSlot = universeData.values[0];
    if (checkSlot == currentValue_) {
        // Keepalive packet
        return;
    }

    if (checkSlot != nextValue_) {
        // Out of order or dropped packet.
        Q_EMIT(dataMismatch(QDateTime::currentDateTimeUtc()));
    }

    currentValue_ = checkSlot;
    ++nextValue_;
}

void ReceiverHandler::HandleSourcesLost(
    sacn::Receiver::Handle handle, uint16_t universe, const std::vector<SacnLostSource> &lostSources)
{
    for (const auto &source : lostSources) {
        if (source.cid == cid_) {
            Q_EMIT(transmitterLost(QDateTime::currentDateTimeUtc()));
        }
    }
}

} // namespace detail

ReceiverWorker::ReceiverWorker(const Config &config, QObject *parent) :
    QObject(parent), config_(config)
{}

void ReceiverWorker::start()
{
    SPDLOG_INFO("Starting receiver on universes {}", config_.universes);

    auto netints = mcastInterfaces(config_.iface);
    for (const auto universe : config_.universes) {
        auto &handler = handlers_.emplace_back(new detail::ReceiverHandler(config_.cid, this));
        connect(
            handler.get(),
            &detail::ReceiverHandler::dataMismatch,
            this,
            &ReceiverWorker::dataMismatch,
            Qt::QueuedConnection);
        connect(
            handler.get(),
            &detail::ReceiverHandler::transmitterLost,
            this,
            &ReceiverWorker::transmitterLost,
            Qt::QueuedConnection);

        sacn::Receiver::Settings settings(universe);
        auto &receiver = receivers_.emplace_back(new sacn::Receiver);
        receiver->Startup(settings, *handler, netints);
    }
}

void ReceiverWorker::stop()
{
    receivers_.clear();
    handlers_.clear();
}

} // namespace sacndeathray
