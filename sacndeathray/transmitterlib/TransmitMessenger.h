/**
 * @file TransmitMessenger.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERLIB_TRANSMITMESSENGER_H
#define SACNDEATHRAY_TRANSMITTERLIB_TRANSMITMESSENGER_H

#include <QNetworkInterface>
#include <QObject>
#include <QWebSocket>

namespace sacndeathray {

/**
 * Handle I/O with the receiver.
 */
class TransmitMessenger : public QObject
{
    Q_OBJECT
public:
    explicit TransmitMessenger(QObject *parent = nullptr);
    [[nodiscard]] const QNetworkInterface &netint() const { return netint_; }
    void setNetint(const QNetworkInterface &netint) { netint_ = netint; }
    [[nodiscard]] uint16_t port() const { return port_; }
    void setPort(const uint16_t port) { port_ = port; }
    void setReceiverAddress(const QHostAddress &receiverAddress)
    {
        receiverAddress_ = receiverAddress;
    }

Q_SIGNALS:
    void receiverConnected(QDateTime timestamp);
    void receiverError(QString msg, QDateTime timestamp);
    void receiverReady(QDateTime timestamp);
    void receiverDisconnected(QDateTime timestamp);

public Q_SLOTS:
    /**
     * Connect to the receiver.
     *
     * @throws std::runtime_error If the selected network interface has no usable IP Address.
     */
    void start();
    void stop();
    void sendHello(const QString &cid, const std::vector<uint16_t> &universes);

private:
    QNetworkInterface netint_;
    uint16_t port_;
    QWebSocket websocket_;
    QHostAddress receiverAddress_;

private Q_SLOTS:
    void onConnected();
    void onMessageReceived(const QByteArray &data);
    void onDisconnected();
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERLIB_TRANSMITMESSENGER_H
