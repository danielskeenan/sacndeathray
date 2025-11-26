/**
 * @file TransmitRunner.cpp
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitRunner.h"

#include "etcpal/cpp/netint.h"
#include <spdlog/spdlog.h>

namespace sacndeathray {

TransmitRunner::TransmitRunner(const TransmitOptions &transmitOptions, QObject *parent) :
    QObject(parent), transmitOptions_(transmitOptions), controller_(new TransmitController(this)),
    messenger_(new TransmitMessenger(this))
{
    connect(messenger_, &TransmitMessenger::receiverError, this, &TransmitRunner::onReceiverError);
}

void TransmitRunner::start()
{
    SPDLOG_INFO("Setting up transmitter");

    // Set interface if required.
    if (!transmitOptions_.netInt.has_value()) {
        try {
            detectCorrectNetInt();
        } catch (std::runtime_error &e) {
            // TODO: Better error reporting.
            Q_EMIT(finished());
        }
    }

    // Setup transmitter.
    controller_->setInterface(*transmitOptions_.netInt);
    controller_->setRate(transmitOptions_.rate);
    controller_->setUniverses(transmitOptions_.universes);

    // Setup messenger.
    messenger_->setReceiverAddress(transmitOptions_.receiverAddress);
    messenger_->setNetint(*transmitOptions_.netInt);
    messenger_->setPort(transmitOptions_.port);

    // Start the process.
    messenger_->start();
    messenger_->sendHello(controller_->getCid(), transmitOptions_.universes);
}

void TransmitRunner::stop()
{
    controller_->stop();
    messenger_->stop();
    Q_EMIT(finished());
}

void TransmitRunner::detectCorrectNetInt()
{
    SPDLOG_DEBUG("Detecting network interface");
    const auto address = etcpal::IpAddr::FromString(
        transmitOptions_.receiverAddress.toString().toStdString());
    const auto destNetInt = etcpal::netint::GetInterfaceForDest(address);
    if (!destNetInt.has_value()) {
        SPDLOG_CRITICAL(
            "No interface specified and one could not be detected: {}",
            destNetInt.error().ToString());
        throw std::runtime_error("No interface specified and one could not be detected");
    }

    auto netInt = QNetworkInterface::interfaceFromIndex(destNetInt->value());
    if (!netInt.isValid()) {
        SPDLOG_CRITICAL("Internal disagreement about network interface indexes.");
        throw std::runtime_error("Internal disagreement about network interface indexes.");
    }

    SPDLOG_INFO("Using {}", netInt.humanReadableName().toStdString());
    transmitOptions_.netInt = netInt;
}

void TransmitRunner::onReceiverError(const QString &message, const QDateTime &timestamp)
{
    SPDLOG_CRITICAL(
        "Receiver error at {}: {}",
        timestamp.toString(Qt::ISODateWithMs).toStdString(),
        message.toStdString());
    Q_EMIT(finished());
}

} // namespace sacndeathray
