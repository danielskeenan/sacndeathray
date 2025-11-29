/**
 * @file TransmitController.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitController.h"
#include "sacndeathray/config.h"
#include <spdlog/spdlog.h>

namespace sacndeathray {

TransmitController::TransmitController(QObject *parent) : QObject(parent) {}

TransmitController::~TransmitController()
{
    stop();
}

void TransmitController::setInterface(const QNetworkInterface &iface)
{
    config_.iface = iface;

    const auto macAddress = [&iface]() {
        std::array<uint8_t, 6> bytes{};
        if (iface.type() != QNetworkInterface::Ethernet) {
            SPDLOG_ERROR("Non-ethernet interfaces are not supported.");
            return bytes;
        }

        const auto hex = iface.hardwareAddress().split(':');
        auto bytesIt = bytes.begin();
        auto hexIt = hex.begin();
        for (; bytesIt != bytes.end() && hexIt != hex.end(); ++bytesIt, ++hexIt) {
            bool ok;
            const auto byte = static_cast<uint8_t>(hexIt->toUInt(&ok, 16));
            if (!ok) {
                SPDLOG_ERROR(
                    "Bad MAC Address representation: {}", iface.hardwareAddress().toStdString());
                return bytes;
            }
            *bytesIt = byte;
        }
        return bytes;
    }();

    config_.cid = etcpal::Uuid::Device(config::kProjectName, macAddress, 0);
}

void TransmitController::setIncrement(const uint8_t increment)
{
    Q_EMIT(requestSetIncrement(increment));
}

void TransmitController::start()
{
    auto *worker = new TransmitWorker(config_);
    worker->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);
    connect(
        this,
        &TransmitController::beginTransmission,
        worker,
        &TransmitWorker::start,
        Qt::QueuedConnection);
    connect(
        this,
        &TransmitController::requestSetIncrement,
        worker,
        &TransmitWorker::setIncrement,
        Qt::QueuedConnection);
    workerThread_.start();
    Q_EMIT(beginTransmission());
}

void TransmitController::stop()
{
    workerThread_.quit();
    workerThread_.wait();
}

} // namespace sacndeathray
