set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR}")
if (TARGET ${PROJECT_NAME}-gui)
  set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}-gui;${PROJECT_DISPLAY_NAME}")
endif ()
set(CPACK_STRIP_FILES On)

# Linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(GNUInstallDirs)
  if (TARGET ${PROJECT_NAME}-gui)
    # .desktop file is filled in with configure-time info
    configure_file("sacndeathray.desktop" "${CMAKE_CURRENT_BINARY_DIR}/sacndeathray.desktop")
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/sacndeathray.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
        COMPONENT gui
    )

    # App icon
    install(
        FILES "${PROJECT_SOURCE_DIR}/fnsolver/gui/resources/dataprobe.png"
        RENAME "fnsolver-gui.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps"
        COMPONENT gui
    )
  endif ()

  # DEB
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS On)
  set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
  set(CPACK_DEB_COMPONENT_INSTALL On)

  # RPM
  set(CPACK_RPM_COMPONENT_INSTALL On)
  set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
endif ()

# Windows
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
  set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_DISPLAY_NAME}")
  set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_DISPLAY_NAME}")
  set(CPACK_NSIS_IGNORE_LICENSE_PAGE On)
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL On)
endif ()

include(CPack)
