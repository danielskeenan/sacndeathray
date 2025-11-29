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

#endif //SACNDEATHRAY_COMMON_MESSAGE_HELPERS_H
