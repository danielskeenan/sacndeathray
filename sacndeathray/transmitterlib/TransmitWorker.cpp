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
#include <unordered_set>
#include <QTimer>

namespace sacndeathray {

TransmitWorker::TransmitWorker(const Config &config, QObject *parent) :
    QObject(parent), config_(config), timer_(new QTimer(this))
{
    connect(timer_, &QTimer::timeout, this, &TransmitWorker::tick);
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
