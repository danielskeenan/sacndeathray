cmake_minimum_required(VERSION 4.1)
project(sacndeathray
        VERSION 1.0.0
        DESCRIPTION "Stress-test sACN networks"
        # TODO: Make a real website.
        HOMEPAGE_URL ""
)
set(PROJECT_DISPLAY_NAME "sACN Death Ray")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_ORGANIZATION_NAME "dankeenan")
set(PROJECT_ORGANIZATION_DOMAIN "dankeenan.org")
string(TIMESTAMP PROJECT_BUILD_TIMESTAMP "%s" UTC)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" log -n1 --format=%H
        OUTPUT_VARIABLE PROJECT_COMMIT_SHA
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_SHA}" PROJECT_COMMIT_SHA)
execute_process(COMMAND "${GIT_EXECUTABLE}" tag --points-at=HEAD
        OUTPUT_VARIABLE PROJECT_COMMIT_TAG
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_TAG}" PROJECT_COMMIT_TAG)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_APP "Build application" ON)
if (BUILD_APP)
    include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

    set(QT_COMPONENTS
            Core
            Svg
            WebSockets
            Widgets
    )
    find_package(Qt6 6.10 COMPONENTS ${QT_COMPONENTS} REQUIRED)
    qt_standard_project_setup(
            I18N_SOURCE_LANGUAGE en_US
    )

    # Log levels
    add_compile_definitions("SPDLOG_ACTIVE_LEVEL=$<IF:$<CONFIG:Debug>,0,2>")

    add_subdirectory(messages)
    add_subdirectory(sacndeathray)

    #    option(BUILD_PACKAGE "Build packages/installers usable on this platform." OFF)
    #    if (BUILD_PACKAGE)
    #        add_subdirectory(install)
    #    endif ()

    include(CTest)
#    if (BUILD_TESTING)
#        add_subdirectory(sacndeathray_test)
#    endif ()
endif ()

#option(BUILD_DOC "Build documentation" ON)
#if (BUILD_DOC)
#    add_subdirectory(doc)
#endif ()
