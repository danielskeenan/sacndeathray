/**
 * @file TransmitRunner.cpp
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitRunner.h"

#include <QTimer>

#include "etcpal/cpp/netint.h"
#include <spdlog/spdlog.h>

namespace sacndeathray {

TransmitRunner::TransmitRunner(const TransmitOptions &transmitOptions, QObject *parent) :
    QObject(parent), transmitOptions_(transmitOptions), controller_(new TransmitController(this)),
    messenger_(new TransmitMessenger(this))
{
    connect(
        messenger_,
        &TransmitMessenger::receiverConnected,
        this,
        &TransmitRunner::onReceiverConnected);
    connect(messenger_, &TransmitMessenger::receiverError, this, &TransmitRunner::onReceiverError);
    connect(messenger_, &TransmitMessenger::receiverReady, this, &TransmitRunner::onReceiverReady);
    connect(
        messenger_,
        &TransmitMessenger::receiverSentResults,
        this,
        &TransmitRunner::onReceiverResults);
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
    messenger_->setPort(transmitOptions_.port);

    // Start the process.
    messenger_->start();
    SPDLOG_INFO("Connecting to receiver...");
    controller_->start();
}

void TransmitRunner::endTest()
{
    messenger_->sendStop();
    controller_->setIncrement(0);
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

void TransmitRunner::onReceiverConnected()
{
    SPDLOG_INFO("Saying hello");
    messenger_->sendHello(controller_->getCid(), transmitOptions_.universes);
}

void TransmitRunner::onReceiverError(const QString &message, const QDateTime &timestamp)
{
    SPDLOG_CRITICAL(
        "Receiver error at {}: {}",
        timestamp.toString(Qt::ISODateWithMs).toStdString(),
        message.toStdString());
    Q_EMIT(finished());
}

void TransmitRunner::onReceiverReady()
{
    // Increment starts at 0 to allow the receiver to listen for us. Once the receiver hears us, begin
    // incrementing the test slot. The receiver sends its ready message once it hears us.
    SPDLOG_INFO("Beginning test for {} second(s)", transmitOptions_.duration.count());
    controller_->setIncrement(1);
    QTimer::singleShot(transmitOptions_.duration, this, &TransmitRunner::endTest);
}

void TransmitRunner::onReceiverResults(
    const QDateTime &transmitterFound, const std::vector<DataMismatch> &dataMismatches)
{
    SPDLOG_INFO("Results ready");
    Q_EMIT(resultsReady(transmitterFound, dataMismatches));
}

} // namespace sacndeathray
