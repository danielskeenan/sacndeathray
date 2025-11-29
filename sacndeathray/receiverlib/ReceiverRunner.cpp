/**
 * @file ReceiverRunner.cpp
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#include "ReceiverRunner.h"
#include <spdlog/spdlog.h>

namespace sacndeathray {

ReceiverRunner::ReceiverRunner(const ReceiverOptions &receiverOptions, QObject *parent) :
    QObject(parent), receiverOptions_(receiverOptions), controller_(new ReceiverController(this)),
    messenger_(new ReceiverMessenger(this))
{
    connect(
        messenger_, &ReceiverMessenger::transmitterError, this, &ReceiverRunner::onTransmitterError);
    connect(
        messenger_, &ReceiverMessenger::transmitterReady, this, &ReceiverRunner::onTransmitterReady);
    connect(
        messenger_, &ReceiverMessenger::transmitterRequestedStop, this, &ReceiverRunner::onEndTest);
    connect(controller_, &ReceiverController::dataMismatch, this, &ReceiverRunner::onDataMismatch);
}

void ReceiverRunner::start()
{
    SPDLOG_INFO("Setting up receiver");

    // Setup receiver.
    controller_->setInterface(receiverOptions_.netInt);

    // Setup messanger.
    messenger_->setNetint(receiverOptions_.netInt);
    messenger_->setPort(receiverOptions_.port);

    // Start the process.
    messenger_->start();
    SPDLOG_INFO("Waiting for transmitter...");
}

void ReceiverRunner::stop()
{
    controller_->stop();
    messenger_->stop();
    Q_EMIT(finished());
}

void ReceiverRunner::onTransmitterError(const QString &message, const QDateTime &timestamp)
{
    SPDLOG_CRITICAL(
        "Transmitter error at {}: {}",
        timestamp.toString(Qt::ISODateWithMs).toStdString(),
        message.toStdString());
    Q_EMIT(finished());
}

void ReceiverRunner::onTransmitterReady(
    const QUuid &cid, const std::vector<uint16_t> &universes, const QDateTime &timestamp)
{
    SPDLOG_INFO("Got transmitter {}", cid.toString().toStdString());

    connect(
        controller_,
        &ReceiverController::transmitterFound,
        this,
        &ReceiverRunner::onTransmitterFound,
        Qt::SingleShotConnection);
    controller_->setCid(cid);
    controller_->setUniverses(universes);
    controller_->start();
    SPDLOG_INFO("Beginning test");
}

void ReceiverRunner::onTransmitterFound(const QDateTime &timestamp)
{
    mismatches_.clear();
    transmitterFound_ = timestamp;
    messenger_->sendReady();
}

void ReceiverRunner::onDataMismatch(uint16_t universe, const QDateTime &timestamp)
{
    mismatches_.emplace_back(universe, timestamp);
    SPDLOG_INFO(
        "Data mismatch: U{} {}", universe, timestamp.toString(Qt::ISODateWithMs).toStdString());
}

void ReceiverRunner::onEndTest()
{
    controller_->stop();
    messenger_->sendResults(transmitterFound_, mismatches_);
    stop();
}

} // namespace sacndeathray
