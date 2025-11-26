set(TARGET deathray_common)

qt_add_library(${TARGET} STATIC
        EtcPalLogHandler.cpp
        EtcPalLogHandler.h
        message_helpers.cpp
        message_helpers.h
        NetInt.cpp
        NetInt.h
        SpdlogQtMessageHandler.cpp
        SpdlogQtMessageHandler.h
)

find_package(flatbuffers CONFIG REQUIRED)
include(sACN)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        EtcPal
        Qt::Core
        Qt::Network
        flatbuffers::flatbuffers
        spdlog::spdlog
)
