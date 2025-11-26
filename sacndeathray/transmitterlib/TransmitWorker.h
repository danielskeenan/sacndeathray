/**
 * @file TransmitWorker.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERLIB_TRANSMITWORKER_H
#define SACNDEATHRAY_TRANSMITTERLIB_TRANSMITWORKER_H

#include <etcpal/cpp/uuid.h>
#include <generator>
#include <sacn/cpp/source.h>
#include <QNetworkInterface>
#include <QObject>

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

private:
    Config config_;
    QTimer *timer_;
    std::unique_ptr<sacn::Source, detail::SourceDeleter> source_;
    std::array<uint8_t, 512> levelBuffer_{};

private Q_SLOTS:
    void tick();
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERLIB_TRANSMITWORKER_H
