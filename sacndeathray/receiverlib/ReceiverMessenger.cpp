/**
 * @file ReceiverMessenger.cpp
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#include "ReceiverMessenger.h"
#include "sacndeathray/common/message_helpers.h"
#include "sacndeathray/messages/cpp/ReceiverMessage.h"
#include "sacndeathray/messages/cpp/TransmitterMessage.h"
#include <sacndeathray/config.h>
#include <spdlog/spdlog.h>
#include <QAbstractSocket>

namespace sacndeathray {

ReceiverMessenger::ReceiverMessenger(QObject *parent) :
    QObject(parent), port_(config::kMessagePort),
    websocketServer_(
        new QWebSocketServer(config::kProjectName, QWebSocketServer::NonSecureMode, this))
{
    connect(
        websocketServer_,
        &QWebSocketServer::newConnection,
        this,
        &ReceiverMessenger::onNewConnection);
    connect(websocketServer_, &QWebSocketServer::closed, this, &ReceiverMessenger::onConnectionClosed);
    connect(websocketServer_, &QWebSocketServer::acceptError, this, &ReceiverMessenger::onSocketError);
    connect(websocketServer_, &QWebSocketServer::serverError, this, &ReceiverMessenger::onServerError);
}

void ReceiverMessenger::start()
{
    const auto hostAddress = decideHostAddress();
    if (hostAddress.isNull()) {
        SPDLOG_CRITICAL(
            "Could not find host address for netint {}", netint_.humanReadableName().toStdString());
        throw std::runtime_error("Could not find host address for netint");
    }
    SPDLOG_INFO("Listening on {}:{}", hostAddress.toString().toStdString(), port_);
    if (!websocketServer_->listen(hostAddress, port_)) {
        SPDLOG_CRITICAL(
            "Could not start websocket server: {}", websocketServer_->errorString().toStdString());
        throw std::runtime_error("Could not start websocket server");
    }
}

void ReceiverMessenger::stop()
{
    if (websocket_) {
        websocket_.reset();
    }
    websocketServer_->close();
}

QHostAddress ReceiverMessenger::decideHostAddress() const
{
    QHostAddress r;
    // Prefer IPv4 before IPv6.
    for (const auto protocol : {QAbstractSocket::IPv4Protocol, QAbstractSocket::IPv6Protocol}) {
        for (const auto &address : netint_.addressEntries()) {
            const auto ip = address.ip();
            if (ip.protocol() == protocol) {
                r = address.ip();
                break;
            }
        }
        if (!r.isNull()) {
            break;
        }
    }

    return r;
}

void ReceiverMessenger::onNewConnection()
{
    auto newWebsocket = websocketServer_->nextPendingConnection();
    if (newWebsocket == nullptr) {
        SPDLOG_CRITICAL(
            "WebsocketServer says there is a new connection but returns nullptr. This is probably "
            "a bug.");
        throw std::runtime_error(
            "WebsocketServer says there is a new connection but returns nullptr. This is probably "
            "a bug.");
    }
    websocket_.reset(newWebsocket);
    connect(
        newWebsocket,
        &QWebSocket::binaryMessageReceived,
        this,
        &ReceiverMessenger::onMessageReceived);
    connect(newWebsocket, &QWebSocket::disconnected, this, &ReceiverMessenger::onConnectionClosed);
    Q_EMIT(transmitterConnected(QDateTime::currentDateTimeUtc()));
}

void ReceiverMessenger::onMessageReceived(const QByteArray &data)
{
    const auto message = message::GetTransmitterMessage(data.constData());
    const auto timestamp = message_helpers::toQDateTime(message->timestamp());

    if (message->val_type() == message::TransmitterMessageVal::Error) {
        const auto errorMessage = message->val_as_Error();
        const auto errorStr = message_helpers::toQString(errorMessage->message());
        Q_EMIT(transmitterError(errorStr, timestamp));
    } else if (message->val_type() == message::TransmitterMessageVal::Hello) {
        const auto *helloMessage = message->val_as_Hello();
        // Require transmitter and receiver run the same version.
        if (std::strcmp(helloMessage->version()->c_str(), config::kProjectVersion)) {
            Q_EMIT(transmitterError(
                tr("Transmitter version (%1) does not match this version (%2).")
                    .arg(QString::fromUtf8(helloMessage->version()->c_str()))
                    .arg(QString::fromUtf8(config::kProjectVersion)),
                QDateTime::currentDateTimeUtc()));
            websocket_->close(QWebSocketProtocol::CloseCodePolicyViolated, "Version mismatch");
        }
        QUuid cid(QAnyStringView(helloMessage->cid()->c_str()));
        std::vector<uint16_t>
            universes(helloMessage->universes()->cbegin(), helloMessage->universes()->cend());
        Q_EMIT(transmitterReady(cid, universes, QDateTime::currentDateTimeUtc()));
    } else if (message->val_type() == message::TransmitterMessageVal::Stop) {
        Q_EMIT(transmitterRequestedStop(timestamp));
    }
}

void ReceiverMessenger::onConnectionClosed()
{
    Q_EMIT(transmitterDisconnected(QDateTime::currentDateTimeUtc()));
    if (websocket_) {
        websocket_.reset();
    }
}

void ReceiverMessenger::onSocketError(QAbstractSocket::SocketError socketError)
{
    const auto socketErrorEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    Q_EMIT(
        transmitterError(socketErrorEnum.valueToKey(socketError), QDateTime::currentDateTimeUtc()));
    if (websocket_) {
        websocket_.reset();
    }
}

void ReceiverMessenger::onServerError(QWebSocketProtocol::CloseCode closeCode)
{
    Q_EMIT(transmitterError(
        tr("Could not establish connection ({})").arg(closeCode), QDateTime::currentDateTimeUtc()));
    if (websocket_) {
        websocket_.reset();
    }
}

void ReceiverMessenger::sendReady()
{
    flatbuffers::FlatBufferBuilder builder;

    const auto timestamp = QDateTime::currentDateTimeUtc();
    auto timestampOff = builder.CreateString(message_helpers::fromQDateTime(timestamp));

    auto readyOff = message::CreateReady(builder);

    auto messageOff = message::CreateReceiverMessage(
        builder, timestampOff, message::ReceiverMessageVal::Ready, readyOff.Union());
    builder.Finish(messageOff);

    const auto data = QByteArray::fromRawData(
        reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
    websocket_->sendBinaryMessage(data);
}

void ReceiverMessenger::sendStop()
{
    flatbuffers::FlatBufferBuilder builder;

    const auto timestamp = QDateTime::currentDateTimeUtc();
    auto timestampOff = builder.CreateString(message_helpers::fromQDateTime(timestamp));

    auto stopOff = message::CreateStop(builder);

    auto messageOff = message::CreateReceiverMessage(
        builder, timestampOff, message::ReceiverMessageVal::Stop, stopOff.Union());
    builder.Finish(messageOff);

    const auto data = QByteArray::fromRawData(
        reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
    websocket_->sendBinaryMessage(data);
}

void ReceiverMessenger::sendResults(
    const QDateTime &transmitterFound, const std::vector<DataMismatch> &dataMismatches)
{
    flatbuffers::FlatBufferBuilder builder;

    const auto timestamp = QDateTime::currentDateTimeUtc();
    auto timestampOff = builder.CreateString(message_helpers::fromQDateTime(timestamp));

    auto transmitterFoundOff = builder.CreateString(
        message_helpers::fromQDateTime(transmitterFound));

    std::vector<flatbuffers::Offset<message::DataMismatch>> dataMismatchOffs;
    for (const auto &dataMismatch : dataMismatches) {
        auto dataMismatchTimestampOff = builder.CreateString(
            message_helpers::fromQDateTime(dataMismatch.timestamp));
        auto dataMismatchOff
            = message::CreateDataMismatch(builder, dataMismatch.universe, dataMismatchTimestampOff);
        dataMismatchOffs.push_back(dataMismatchOff);
    }
    auto dataMismatchesOff = builder.CreateVector(dataMismatchOffs);

    auto resultsOff = message::CreateResults(builder, transmitterFoundOff, dataMismatchesOff);

    auto messageOff = message::CreateReceiverMessage(
        builder, timestampOff, message::ReceiverMessageVal::Results, resultsOff.Union());
    builder.Finish(messageOff);

    const auto data = QByteArray::fromRawData(
        reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
    websocket_->sendBinaryMessage(data);
}

} // namespace sacndeathray
