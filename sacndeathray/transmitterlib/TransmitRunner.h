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
 *
 * Order of operation:
 *
 * - Call start(). The messenger will attempt to connect to receiver defined in @p transmitOptions.
 *   Once connected...
 * - Send a Hello message (Hello.fbs) to tell the receiver our details. At the same time, begin
 *   transmitting on all universes but don't start the test sequence. This will allow...
 * - Receiver will open an sACN Receiver and look for our CID transmitting. Once it sees the
 *   transmitter...
 * - Receiver will send a Ready message (Ready.fbs). The whole process to this point should only
 *   take a couple seconds!
 * - Once we get the Ready message, begin transmitting the test sequence for the duration defined in
 *   @p transmitOptions.
 * - When the duration has run, ask the receiver for results and stop transmitting.
 * - Emit resultsReady() with the results from the receiver.
 *
 * @see ReceiverRunner
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
