/**
 * @file TransmitWorker.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERLIB_TRANSMITWORKER_H
#define SACNDEATHRAY_TRANSMITTERLIB_TRANSMITWORKER_H

#include <chrono>
#include <etcpal/cpp/uuid.h>
#include <generator>
#include <sacn/cpp/source.h>
#include <QNetworkInterface>
#include <QObject>
#include <QThread>
#include <QTimer>

namespace sacndeathray {

namespace detail {
struct SourceDeleter
{
    void operator()(sacn::Source *ptr) const
    {
        if (ptr != nullptr) {
            ptr->Shutdown();
            delete ptr;
        }
    }
};
} // namespace detail

/**
 * Count the number of times tick() is called every samplePeriod() ms.
 */
class FpsCounter : public QThread
{
    Q_OBJECT
public:
    using QThread::QThread;

    /**
     * Call every frame. This function is thread-safe.
     */
    void tick() { ++ticks_; }

    void run() override;

    [[nodiscard]] std::chrono::milliseconds samplePeriod() const { return samplePeriod_; }
    void setSamplePeriod(const std::chrono::milliseconds &samplePeriod)
    {
        samplePeriod_ = samplePeriod;
    }

Q_SIGNALS:
    /**
     * Emitted roughly every samplePeriod() with the current measured FPS.
     * @param fps
     */
    void fpsReady(double fps);

private:
    std::chrono::milliseconds samplePeriod_{5000};
    std::atomic<unsigned long> ticks_;
    std::chrono::steady_clock::time_point lastSampleTime_;
    std::chrono::steady_clock::time_point nextSampleTime_;
};

/**
 * @see TransmitController
 */
class TransmitWorker : public QObject
{
    Q_OBJECT
public:
    struct Config
    {
        /** Network interface to use */
        QNetworkInterface iface;
        /** Transmit rate (Hertz) */
        double rate = 22;
        /** Transmitter CID */
        etcpal::Uuid cid;
        /** Universes to transmit on */
        std::vector<uint16_t> universes;
    };
    explicit TransmitWorker(const Config &config, QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * Start the transmitter.
     *
     * @throws std::runtime_error When the transmitter cannot be started.
     */
    void start();

    /**
     * Stop the transmitter.
     */
    void stop();

    /**
     * Change the increment amount on the test slot.
     * @param increment
     */
    void setIncrement(uint8_t increment);

private:
    Config config_;
    std::atomic<uint8_t> increment_{0};
    QTimer *timer_;
    std::unique_ptr<sacn::Source, detail::SourceDeleter> source_;
    std::array<uint8_t, 512> levelBuffer_{};
    FpsCounter *fpsCounter_;

private Q_SLOTS:
    void tick();
    void fpsUpdated(double fps) const;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERLIB_TRANSMITWORKER_H
