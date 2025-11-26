set(TARGET deathray_transmitterlib)

qt_add_library(${TARGET} STATIC
        TransmitController.cpp
        TransmitController.h
        TransmitMessenger.cpp
        TransmitMessenger.h
        TransmitOptions.h
        TransmitRunner.cpp
        TransmitRunner.h
        TransmitWorker.cpp
        TransmitWorker.h
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
