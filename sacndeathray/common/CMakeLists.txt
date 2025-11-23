set(TARGET deathray_common)

qt_add_library(${TARGET} STATIC
        EtcPalLogHandler.cpp
        EtcPalLogHandler.h
        SpdlogQtMessageHandler.cpp
        SpdlogQtMessageHandler.h
)

include(sACN)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Qt::Core
        EtcPal
        spdlog::spdlog
)
