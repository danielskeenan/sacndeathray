/**
 * @file ReceiverRunner.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_RECEIVERLIB_RECEIVERRUNNER_H
#define SACNDEATHRAY_RECEIVERLIB_RECEIVERRUNNER_H

#include "../common/DataMismatch.h"
#include "ReceiverController.h"
#include "ReceiverMessenger.h"
#include "ReceiverOptions.h"
#include <QObject>

namespace sacndeathray {

/**
 * Run the receiver.
 *
 * @see TransmitRunner
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
    QDateTime transmitterFound_;
    std::vector<DataMismatch> mismatches_;

private Q_SLOTS:
    void onTransmitterError(const QString &message, const QDateTime &timestamp);
    void onTransmitterReady(
        const QUuid &cid, const std::vector<uint16_t> &universes, const QDateTime &timestamp);
    void onTransmitterFound(const QDateTime &timestamp);
    void onDataMismatch(uint16_t universe, const QDateTime &timestamp);
    void onEndTest();
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_RECEIVERLIB_RECEIVERRUNNER_H
