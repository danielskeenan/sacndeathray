/**
 * @file ResultsPrinter.cpp
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#include "ResultsPrinter.h"
#include <iostream>

namespace sacndeathray {

void ResultsPrinter::printResults(
    const QDateTime &transmitterFound, const std::vector<DataMismatch> &dataMismatches)
{
    std::cout << qPrintable(tr("---------------------\n      RESULTS      \n---------------------"))
              << std::endl
              << qPrintable(tr("Began at %1").arg(transmitterFound.toLocalTime().toString()))
              << std::endl;

    if (dataMismatches.empty()) {
        std::cout << qPrintable(tr("No errors detected!")) << std::endl;
    } else {
        std::cout << qPrintable(
            tr("%n error(s) detected:", "", static_cast<int>(dataMismatches.size()))
                .arg(dataMismatches.size()))
                  << std::endl;
        for (const auto &mismatch : dataMismatches) {
            std::cout << qPrintable(tr("U%1: ").arg(mismatch.universe))
                      << qPrintable(mismatch.timestamp.toLocalTime().toString()) << std::endl;
        }
    }
}

} // namespace sacndeathray
