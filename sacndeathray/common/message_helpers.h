/**
 * @file message_helpers.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_MESSAGE_HELPERS_H
#define SACNDEATHRAY_COMMON_MESSAGE_HELPERS_H

#include <flatbuffers/string.h>
#include <QDateTime>
#include <QString>

namespace sacndeathray::message_helpers {

QString toQString(const flatbuffers::String *str);

QDateTime toQDateTime(const flatbuffers::String *str);
std::string fromQDateTime(const QDateTime &timestamp);

} // namespace sacndeathray::message_helpers

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define DEATHRAY_LOG_MESSAGE_SENT(data) \
    { \
        const auto message = message::GetMessage(data.data()); \
        SPDLOG_DEBUG( \
            "Sent {} message with timestamp {}", \
            message::EnumNameMessageVal(message->val_type()), \
            message->timestamp()->c_str()); \
    }
#define DEATHRAY_LOG_MESSAGE_RECEIVED(message) \
    SPDLOG_DEBUG( \
        "Received {} message with timestamp {}", \
        message::EnumNameMessageVal(message->val_type()), \
        message->timestamp()->c_str())
#else
#define DEATHRAY_LOG_MESSAGE_SENT(...) (void) 0
#define DEATHRAY_LOG_MESSAGE_RECEIVED(...) (void) 0
#endif

#endif //SACNDEATHRAY_COMMON_MESSAGE_HELPERS_H
