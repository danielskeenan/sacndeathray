/**
 * @file NetInt.h
 *
 * @author Dan Keenan
 * @date 11/25/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_NETINT_H
#define SACNDEATHRAY_COMMON_NETINT_H

#include <vector>
#include <QNetworkInterface>

namespace sacndeathray {

/**
 * Get a list of usable network interfaces.
 * @return
 */
std::vector<QNetworkInterface> getNetInts();

/**
 * Print network interfaces to @p out.
 * @param out
 */
void printNetInts (std::ostream& out);

} // namespace sacndeathray

#endif //SACNDEATHRAY_COMMON_NETINT_H
