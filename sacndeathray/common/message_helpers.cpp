/**
 * @file message_helpers.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "message_helpers.h"

namespace sacndeathray::message_helpers {

QString toQString(const flatbuffers::String *const str)
{
    return QString::fromUtf8(str->c_str());
}

QDateTime toQDateTime(const flatbuffers::String *const str)
{
    return QDateTime::fromString(toQString(str), Qt::ISODateWithMs);
}

std::string fromQDateTime(const QDateTime &timestamp)
{
    return timestamp.toString(Qt::ISODateWithMs).toStdString();
}

} // namespace sacndeathray::message_helpers
