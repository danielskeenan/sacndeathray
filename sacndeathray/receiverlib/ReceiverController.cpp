/**
 * @file ReceiverController.cpp
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#include "ReceiverController.h"
#include "sacndeathray/common/McastInterfaces.h"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace sacndeathray {

namespace detail {

void ReceiverHandler::HandleUniverseData(
    sacn::Receiver::Handle receiverHandle,
    const etcpal::SockAddr &sourceAddr,
    const SacnRemoteSource &sourceInfo,
    const SacnRecvUniverseData &universeData)
{
    if (sourceInfo.cid != cid_ || universeData.start_code != 0) {
        // Don't care about this source.
        return;
    }
    if (!found_) {
        found_ = true;
        Q_EMIT(transmitterFound(QDateTime::currentDateTimeUtc()));
    }

    const auto checkSlot = universeData.values[0];
    if (checkSlot == currentValue_) {
        // Keepalive packet
        return;
    }

    if (checkSlot != nextValue_) {
        // Out of order or dropped packet.
        Q_EMIT(dataMismatch(universe_, QDateTime::currentDateTimeUtc()));
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
            found_ = false;
            return;
        }
    }
}

} // namespace detail

ReceiverController::ReceiverController(QObject *parent) : QObject(parent) {}

void ReceiverController::setCid(const QUuid &cid)
{
    setCid(etcpal::Uuid::FromString(cid.toString(QUuid::WithoutBraces).toStdString()));
}

void ReceiverController::start()
{
    SPDLOG_INFO("Starting receiver on universes {}", config_.universes);

    auto netints = mcastInterfaces(config_.iface);
    for (const auto universe : config_.universes) {
        auto &handler = handlers_.emplace_back(
            new detail::ReceiverHandler(universe, config_.cid, this));
        connect(
            handler.get(),
            &detail::ReceiverHandler::dataMismatch,
            this,
            &ReceiverController::dataMismatch,
            Qt::QueuedConnection);
        connect(
            handler.get(),
            &detail::ReceiverHandler::transmitterFound,
            this,
            &ReceiverController::transmitterFound,
            Qt::QueuedConnection);
        connect(
            handler.get(),
            &detail::ReceiverHandler::transmitterLost,
            this,
            &ReceiverController::transmitterLost,
            Qt::QueuedConnection);

        sacn::Receiver::Settings settings(universe);
        auto &receiver = receivers_.emplace_back(new sacn::Receiver);
        receiver->Startup(settings, *handler, netints);
    }
}

void ReceiverController::stop()
{
    receivers_.clear();
    handlers_.clear();
}

} // namespace sacndeathray
