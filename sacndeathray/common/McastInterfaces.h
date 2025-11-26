/**
 * @file McastInterfaces.h
 *
 * @author Dan Keenan
 * @date 11/26/2025
 * @copyright GPL-3.0-or-later
 */

#ifndef SACNDEATHRAY_COMMON_MCASTINTERFACES_H
#define SACNDEATHRAY_COMMON_MCASTINTERFACES_H

#include <sacn/common.h>
#include <vector>
#include <QNetworkInterface>

namespace sacndeathray {

/**
 * Create a list of SacnMcastInterface objects for the given QNetworkInterface.
 * @param iface
 * @return
 */
std::vector<SacnMcastInterface> mcastInterfaces(const QNetworkInterface &iface);

}

#endif //SACNDEATHRAY_COMMON_MCASTINTERFACES_H
