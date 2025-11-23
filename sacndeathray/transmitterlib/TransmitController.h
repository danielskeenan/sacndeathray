/**
 * @file TransmitController.h
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERLIB_TRANSMITCONTROLLER_H
#define SACNDEATHRAY_TRANSMITTERLIB_TRANSMITCONTROLLER_H

#include <QObject>
#include <QThread>

#include "TransmitWorker.h"

namespace sacndeathray {

/**
 * Control transmitter state.
 */
class TransmitController : public QObject
{
    Q_OBJECT
public:
    explicit TransmitController(QObject *parent = nullptr);
    ~TransmitController() override;
    void setInterface(const QNetworkInterface &iface);
    void setRate(const double rate) { config_.rate = rate; }
    [[nodiscard]] std::string getCid() const { return config_.cid.ToString(); }
    void setUniverses(const std::vector<uint16_t> &universes) { config_.universes = universes; }

Q_SIGNALS:
    /** @internal */
    void beginTransmission();

public Q_SLOTS:
    void start();
    void stop();

private:
    QThread workerThread_;
    TransmitWorker::Config config_;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERLIB_TRANSMITCONTROLLER_H
