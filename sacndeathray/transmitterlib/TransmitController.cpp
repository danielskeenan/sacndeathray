/**
 * @file TransmitController.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitController.h"

namespace sacndeathray {

TransmitController::TransmitController(QObject *parent) : QObject(parent) {}

TransmitController::~TransmitController()
{
    stop();
}

std::string TransmitController::getCid() const
{
    return config_.cid.ToString();
}

void TransmitController::start()
{
    auto *worker = new TransmitWorker(config_, this);
    worker->moveToThread(&workerThread_);
    connect(&workerThread_, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &TransmitController::beginTransmission, worker, &TransmitWorker::start, Qt::QueuedConnection);
    workerThread_.start();
    Q_EMIT(beginTransmission());
}

void TransmitController::stop()
{
    workerThread_.quit();
    workerThread_.wait();
}

} // namespace sacndeathray
