/**
 * @file SpdlogQtMessageHandler.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "SpdlogQtMessageHandler.h"
#include <spdlog/spdlog.h>
#include <QString>

namespace sacndeathray {
spdlog::level::level_enum spdlogLevel(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return spdlog::level::debug;
    case QtWarningMsg:
        return spdlog::level::warn;
    case QtCriticalMsg:
        return spdlog::level::err;
    case QtFatalMsg:
        return spdlog::level::critical;
    case QtInfoMsg:
        return spdlog::level::info;
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
    case QtSystemMsg:
        return spdlogLevel(QtMsgType::QtCriticalMsg);
#endif
    }
    SPDLOG_ERROR("Unknown Qt log type {}", static_cast<std::underlying_type_t<QtMsgType>>(type));
    return spdlog::level::err;
}

spdlog::source_loc spdlogSourceLoc(const QMessageLogContext &context)
{
    if (context.file == nullptr || context.line == 0 || context.function == nullptr) {
        return {};
    }
    return {context.file, context.line, context.function};
}

void spdlogQtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const auto level = spdlogLevel(type);
    const auto sourceLoc = spdlogSourceLoc(context);
    spdlog::log(sourceLoc, level, msg.toStdString());
}

} // namespace sacndeathray
