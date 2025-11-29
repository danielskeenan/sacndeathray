/**
 * @file ReceiverController.h
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERLIB_RECEIVERCONTROLLER_H
#define SACNDEATHRAY_RECEIVERLIB_RECEIVERCONTROLLER_H

#include <sacn/cpp/receiver.h>
#include <vector>
#include <QDateTime>
#include <QNetworkInterface>
#include <QObject>
#include <QUuid>

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
    explicit ReceiverHandler(uint16_t universe, const etcpal::Uuid cid, QObject *parent = nullptr) :
        QObject(parent), universe_(universe), cid_(cid)
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
    void transmitterFound(QDateTime timestamp);
    void transmitterLost(QDateTime timestamp);
    void dataMismatch(uint16_t universe, QDateTime timestamp);

private:
    bool found_ = false;
    uint16_t universe_;
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
 * Manage the sACN Receiver.
 */
class ReceiverController : public QObject
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
    explicit ReceiverController(QObject *parent = nullptr);
    void setInterface(const QNetworkInterface &iface) { config_.iface = iface; }
    void setCid(const etcpal::Uuid &cid) { config_.cid = cid; }
    void setCid(const QUuid &cid);
    void setUniverses(const std::vector<uint16_t> &universes) { config_.universes = universes; }

Q_SIGNALS:
    void transmitterFound(QDateTime timestamp);
    void transmitterLost(QDateTime timestamp);
    void dataMismatch(uint16_t universe, QDateTime timestamp);

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

#endif //SACNDEATHRAY_RECEIVERLIB_RECEIVERCONTROLLER_H
