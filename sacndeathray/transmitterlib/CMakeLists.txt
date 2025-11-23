set(TARGET deathray_transmitter)

qt_add_library(${TARGET} STATIC
        TransmitController.cpp
        TransmitController.h
        TransmitWorker.cpp
        TransmitWorker.h
)

find_package(flatbuffers CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
include(sACN)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET} PUBLIC
        Qt::Core
        Qt::WebSockets
        flatbuffers::flatbuffers
        fmt::fmt
        sACN
        spdlog::spdlog
)
