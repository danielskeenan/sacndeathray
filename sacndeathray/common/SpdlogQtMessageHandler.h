/**
 * @file SpdlogQtMessageHandler.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_SPDLOGQTMESSAGEHANDLER_H
#define SACNDEATHRAY_COMMON_SPDLOGQTMESSAGEHANDLER_H

#include <QtLogging>

namespace sacndeathray {

class SpdlogQtMessageHandler
{
    /**
     * Connect QtLogging to spdlog.
     *
     * @see qInstallMessageHandler()
     */
    void spdlogQtMessageHandler(
        QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_COMMON_SPDLOGQTMESSAGEHANDLER_H
