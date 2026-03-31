/**
 * @file TransmitWorker.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitWorker.h"
#include "sacndeathray/common/McastInterfaces.h"
#include "sacndeathray/config.h"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <QTimer>

namespace sacndeathray {

void FpsCounter::run()
{
    lastSampleTime_ = std::chrono::steady_clock::now();
    nextSampleTime_ = lastSampleTime_ + samplePeriod_;
    while (!isInterruptionRequested()) {
        const auto now = std::chrono::steady_clock::now();
        if (now < nextSampleTime_) {
            continue;
        }
        const std::chrono::duration<double> sampleDuration(now - lastSampleTime_);
        const double fps = ticks_.exchange(0) / sampleDuration.count();
        Q_EMIT(fpsReady(fps));
        lastSampleTime_ = now;
        nextSampleTime_ = now + samplePeriod_;
    }
}

TransmitWorker::TransmitWorker(const Config &config, QObject *parent) :
    QObject(parent), config_(config), timer_(new QTimer(this)), fpsCounter_(new FpsCounter(this))
{
    timer_->setTimerType(Qt::PreciseTimer);
    connect(timer_, &QTimer::timeout, this, &TransmitWorker::tick);
    connect(
        fpsCounter_, &FpsCounter::fpsReady, this, &TransmitWorker::fpsUpdated, Qt::QueuedConnection);
}

void TransmitWorker::start()
{
    SPDLOG_INFO("Staring transmitter {} on universes {}", config_.cid.ToString(), config_.universes);
    sacn::Source::Settings sourceSettings(config_.cid, config::kProjectDisplayName);
    // Processed in TransmitWorker::tick; this allows us to control transmit timing.
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
    timer_->start(static_cast<int>(std::lround((1 / config_.rate) * 1000)));
    fpsCounter_->start();
}

void TransmitWorker::stop()
{
    timer_->stop();
    fpsCounter_->requestInterruption();
    fpsCounter_->wait();
    source_.reset();
}

void TransmitWorker::setIncrement(const uint8_t increment)
{
    increment_ = increment;
}

void TransmitWorker::tick()
{
    if (QThread::currentThread()->isInterruptionRequested()) {
        stop();
        QThread::currentThread()->quit();
        return;
    }
    for (const auto univ : config_.universes) {
        source_->UpdateLevels(univ, levelBuffer_.data(), levelBuffer_.size());
    }
    source_->ProcessManual(sacn::Source::TickMode::kProcessLevelsOnly);
    fpsCounter_->tick();
    // Overflow wraps value back to 0.
    levelBuffer_[0] += increment_.load();
}

void TransmitWorker::fpsUpdated(double fps) const
{
    SPDLOG_INFO("Current transmit rate: {} Hz", fps);
}

} // namespace sacndeathray
