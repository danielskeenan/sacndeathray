/**
 * @file ReceiverMessenger.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERLIB_RECEIVERMESSENGER_H
#define SACNDEATHRAY_RECEIVERLIB_RECEIVERMESSENGER_H

#include "sacndeathray/common/WebSocketDeleter.h"
#include <QNetworkInterface>
#include <QObject>
#include <QUuid>
#include <QWebSocketServer>

namespace sacndeathray {

/**
 * Handle I/O with the transmitter.
 */
class ReceiverMessenger : public QObject
{
    Q_OBJECT
public:
    explicit ReceiverMessenger(QObject *parent = nullptr);
    [[nodiscard]] const QNetworkInterface &netint() const { return netint_; }
    void setNetint(const QNetworkInterface &netint) { netint_ = netint; }
    [[nodiscard]] uint16_t port() const { return port_; }
    void setPort(const uint16_t port) { port_ = port; }

Q_SIGNALS:
    void transmitterConnected(QDateTime timestamp);
    void transmitterError(QString msg, QDateTime timestamp);
    void transmitterReady(QUuid cid, std::vector<uint16_t> universes, QDateTime timestamp);
    void transmitterDisconnected(QDateTime timestamp);

public Q_SLOTS:
    /**
     * Begin listening for transmitters.
     *
     * @throws std::runtime_error If the selected network interface has no usable IP Address or if
     * the websocket server could not be started.
     */
    void start();
    void stop();
    void sendReady();

private:
    QNetworkInterface netint_;
    uint16_t port_;
    QWebSocketServer *websocketServer_;
    std::unique_ptr<QWebSocket, WebSocketDeleter> websocket_;

    [[nodiscard]] QHostAddress decideHostAddress() const;

private Q_SLOTS:
    void onNewConnection();
    void onMessageReceived(const QByteArray &data);
    void onConnectionClosed();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onServerError(QWebSocketProtocol::CloseCode closeCode);
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_RECEIVERLIB_RECEIVERMESSENGER_H
