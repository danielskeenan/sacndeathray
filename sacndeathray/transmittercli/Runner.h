/**
 * @file Runner.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERCLI_RUNNER_H
#define SACNDEATHRAY_TRANSMITTERCLI_RUNNER_H

#include "ProgramOptions.h"
#include "sacndeathray/transmitterlib/TransmitController.h"
#include "sacndeathray/transmitterlib/TransmitMessenger.h"
#include <QObject>

namespace sacndeathray {

class Runner : public QObject
{
    Q_OBJECT
public:
    explicit Runner(const ProgramOptions &programOptions, QObject *parent = nullptr) :
        QObject(parent), programOptions_(programOptions), controller_(new TransmitController(this)),
        messenger_(new TransmitMessenger(this))
    {}

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void start();

private:
    ProgramOptions programOptions_;
    TransmitController *controller_;
    TransmitMessenger *messenger_;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERCLI_RUNNER_H
