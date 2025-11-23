/**
 * @file TransmitMessenger.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "TransmitMessenger.h"
#include "sacndeathray/common/message_helpers.h"
#include "sacndeathray/messages/cpp/Hello.h"
#include "sacndeathray/messages/cpp/Message.h"
#include <flatbuffers/flatbuffers.h>
#include <sacndeathray/config.h>
#include <spdlog/spdlog.h>

namespace sacndeathray {

TransmitMessenger::TransmitMessenger(QObject *parent) : QObject(parent), port_(config::kMessagePort)
{
    connect(&websocket_, &QWebSocket::connected, this, &TransmitMessenger::onConnected);
    connect(&websocket_, &QWebSocket::disconnected, this, &TransmitMessenger::onDisconnected);
    connect(
        &websocket_,
        &QWebSocket::binaryMessageReceived,
        this,
        &TransmitMessenger::onMessageReceived);
}

void TransmitMessenger::start()
{
    const auto url = [this]() {
        QUrl r;
        r.setScheme("ws");
        r.setHost(receiverAddress_.toString());
        r.setPort(port_);
        return r;
    }();
    websocket_.open(url);
}

void TransmitMessenger::stop()
{
    websocket_.close(QWebSocketProtocol::CloseCodeNormal, "Stop requested");
}

void TransmitMessenger::onConnected()
{
    Q_EMIT(receiverConnected(QDateTime()));
}

void TransmitMessenger::onMessageReceived(const QByteArray &data)
{
    const auto message = message::GetMessage(data.constData());
    const auto timestamp = message_helpers::toQDateTime(message->timestamp());
    DEATHRAY_LOG_MESSAGE_RECEIVED(message);

    if (message->val_type() == message::MessageVal::Error) {
        const auto errorMessage = message->val_as_Error();
        const auto errorStr = message_helpers::toQString(errorMessage->message());
        Q_EMIT(receiverError(errorStr, timestamp));
    } else if (message->val_type() == message::MessageVal::Ready) {
        Q_EMIT(receiverReady(timestamp));
    }
}

void TransmitMessenger::onDisconnected()
{
    Q_EMIT(receiverDisconnected(QDateTime()));
}

void TransmitMessenger::sendHello(const QString &cid, const std::vector<uint16_t> &universes)
{
    flatbuffers::FlatBufferBuilder builder;

    const auto timestamp = QDateTime();
    auto timestampOff = builder.CreateString(message_helpers::fromQDateTime(timestamp));
    auto versionOff = builder.CreateString(config::kProjectVersion);
    auto cidOff = builder.CreateString(cid.toStdString());
    auto universesOff = builder.CreateVector(universes);

    auto helloOff = message::CreateHello(builder, versionOff, cidOff, universesOff);

    auto messageOff
        = message::CreateMessage(builder, timestampOff, message::MessageVal::Hello, helloOff.Union());
    builder.Finish(messageOff);

    const auto data = QByteArray::fromRawData(
        reinterpret_cast<const char *>(builder.GetBufferPointer()), builder.GetSize());
    DEATHRAY_LOG_MESSAGE_SENT(data);
    websocket_.sendBinaryMessage(data);
}

} // namespace sacndeathray
