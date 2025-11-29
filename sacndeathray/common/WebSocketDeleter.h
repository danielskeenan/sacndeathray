/**
 * @file WebSocketDeleter.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_WEBSOCKETDELETER_H
#define SACNDEATHRAY_COMMON_WEBSOCKETDELETER_H

#include <QWebSocket>

namespace sacndeathray {

/**
 * Deleter for QWebSocket objects.
 */
struct WebSocketDeleter
{
    void operator()(QWebSocket* ptr) const
    {
        if (ptr != nullptr) {
            ptr->close(QWebSocketProtocol::CloseCodeGoingAway);
            ptr->deleteLater();
        }
    }
};

}

#endif //SACNDEATHRAY_COMMON_WEBSOCKETDELETER_H
