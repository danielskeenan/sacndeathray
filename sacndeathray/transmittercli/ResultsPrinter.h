/**
 * @file ResultsPrinter.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_TRANSMITTERCLI_RESULTSPRINTER_H
#define SACNDEATHRAY_TRANSMITTERCLI_RESULTSPRINTER_H

#include "sacndeathray/common/DataMismatch.h"
#include <QObject>

namespace sacndeathray {

/**
 * Print results to stdout.
 */
class ResultsPrinter : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

public Q_SLOTS:
    void printResults(
        const QDateTime &transmitterFound, const std::vector<DataMismatch> &dataMismatches);
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_TRANSMITTERCLI_RESULTSPRINTER_H
