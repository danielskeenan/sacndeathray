/**
 * @file DataMismatch.h
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_DATAMISMATCH_H
#define SACNDEATHRAY_COMMON_DATAMISMATCH_H

#include <cstdint>
#include <QDateTime>

namespace sacndeathray {

struct DataMismatch
{
    uint16_t universe;
    QDateTime timestamp;
};

} // namespace sacndeathray

#endif //SACNDEATHRAY_COMMON_DATAMISMATCH_H
