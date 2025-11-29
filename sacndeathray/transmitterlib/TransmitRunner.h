/**
 * @file TransmitRunner.h
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERLIB_TRANSMITRUNNER_H
#define SACNDEATHRAY_TRANSMITTERLIB_TRANSMITRUNNER_H

#include "TransmitController.h"
#include "TransmitMessenger.h"
#include "TransmitOptions.h"
#include <QThread>

namespace sacndeathray {

/**
 * Run the transmitter.
 */
class TransmitRunner : public QObject
{
    Q_OBJECT
public:
    explicit TransmitRunner(const TransmitOptions &transmitOptions, QObject *parent = nullptr);

Q_SIGNALS:
    void finished();
    void resultsReady(
        const QDateTime &transmitterFound, const std::vector<DataMismatch> &dataMismatches);

public Q_SLOTS:
    void start();
    void endTest();
    void stop();

private:
    TransmitOptions transmitOptions_;
    TransmitController *controller_;
    TransmitMessenger *messenger_;

    void detectCorrectNetInt();

private Q_SLOTS:
    void onReceiverConnected();
    void onReceiverError(const QString &message, const QDateTime &timestamp);
    void onReceiverReady();
    void onReceiverResults(
        const QDateTime &transmitterFound, const std::vector<DataMismatch> &dataMismatches);
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERLIB_TRANSMITRUNNER_H
