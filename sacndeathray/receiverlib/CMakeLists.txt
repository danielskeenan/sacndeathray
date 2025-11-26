set(TARGET deathray_receiverlib)

qt_add_library(${TARGET} STATIC
        ReceiverWorker.cpp
        ReceiverWorker.h
)

find_package(fmt CONFIG REQUIRED)
include(sACN)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Qt::Core
        Qt::WebSockets
        deathray_common
        deathray_messages
        fmt::fmt
        sACN
        spdlog::spdlog
)
