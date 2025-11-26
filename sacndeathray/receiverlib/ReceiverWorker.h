/**
 * @file ReceiverWorker.h
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERLIB_RECEIVERWORKER_H
#define SACNDEATHRAY_RECEIVERLIB_RECEIVERWORKER_H

#include <sacn/cpp/receiver.h>
#include <vector>
#include <QDateTime>
#include <QNetworkInterface>
#include <QObject>

namespace sacndeathray {

namespace detail {
struct ReceiverDeleter
{
    void operator()(sacn::Receiver *ptr) const
    {
        if (ptr != nullptr) {
            ptr->Shutdown();
            delete ptr;
        }
    }
};

/**
 * Packet handler for the sACN receiver.
 */
class ReceiverHandler : public QObject, public sacn::Receiver::NotifyHandler
{
    Q_OBJECT
public:
    explicit ReceiverHandler(const etcpal::Uuid cid, QObject *parent = nullptr) :
        QObject(parent), cid_(cid)
    {}

    /** @internal */
    void HandleUniverseData(
        sacn::Receiver::Handle receiverHandle,
        const etcpal::SockAddr &sourceAddr,
        const SacnRemoteSource &sourceInfo,
        const SacnRecvUniverseData &universeData) override;
    /** @internal */
    void HandleSourcesLost(
        sacn::Receiver::Handle handle,
        uint16_t universe,
        const std::vector<SacnLostSource> &lostSources) override;

Q_SIGNALS:
    void transmitterLost(QDateTime timestamp);
    void dataMismatch(QDateTime timestamp);

private:
    etcpal::Uuid cid_;
    uint8_t currentValue_ = 0;
    uint8_t nextValue_ = currentValue_ + 1;
};

struct ReceiverHandlerDeleter
{
    void operator()(ReceiverHandler *ptr) const
    {
        if (ptr != nullptr) {
            ptr->deleteLater();
        }
    }
};

} // namespace detail

/**
 * @see ReceiverController
 */
class ReceiverWorker : public QObject
{
    Q_OBJECT
public:
    struct Config
    {
        /** Network interface to use */
        QNetworkInterface iface;
        /** Transmitter CID */
        etcpal::Uuid cid;
        /** Universes to listen on */
        std::vector<uint16_t> universes;
    };
    explicit ReceiverWorker(const Config &config, QObject *parent = nullptr);

Q_SIGNALS:
    void transmitterLost(QDateTime timestamp);
    void dataMismatch(QDateTime timestamp);

public Q_SLOTS:
    /**
     * Start the receiver.
     *
     * @throws std::runtime_error When the receiver cannot be started.
     */
    void start();

    /**
     * Stop the receiver.
     */
    void stop();

private:
    Config config_;
    std::vector<std::unique_ptr<sacn::Receiver, detail::ReceiverDeleter>> receivers_;
    std::vector<std::unique_ptr<detail::ReceiverHandler, detail::ReceiverHandlerDeleter>> handlers_;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_RECEIVERLIB_RECEIVERWORKER_H
