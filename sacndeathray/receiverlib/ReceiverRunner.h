/**
 * @file ReceiverRunner.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERLIB_RECEIVERRUNNER_H
#define SACNDEATHRAY_RECEIVERLIB_RECEIVERRUNNER_H

#include "ReceiverController.h"
#include "ReceiverMessenger.h"
#include "ReceiverOptions.h"
#include <QObject>

namespace sacndeathray {

/**
 * Run the receiver.
 */
class ReceiverRunner : public QObject
{
    Q_OBJECT
public:
    explicit ReceiverRunner(const ReceiverOptions &receiverOptions, QObject *parent = nullptr);

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void start();
    void stop();

private:
    ReceiverOptions receiverOptions_;
    ReceiverController *controller_;
    ReceiverMessenger *messenger_;

private Q_SLOTS:
    void onTransmitterError(const QString &message, const QDateTime &timestamp);
    void onTransmitterReady(
        const QUuid &cid, const std::vector<uint16_t> &universes, const QDateTime &timestamp);
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_RECEIVERLIB_RECEIVERRUNNER_H
