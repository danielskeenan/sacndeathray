set(TARGET "sacndeathray_receiver")

qt_add_executable(${TARGET}
        main.cpp
        ProgramOptions.h
)

target_link_libraries(${TARGET} PRIVATE
        deathray_receiverlib
)

qt_add_translations(${TARGET}
        SOURCE_TARGETS
        ${TARGET}
        deathray_receiverlib
        deathray_common
        TS_FILE_DIR translations
)

install(TARGETS ${TARGET}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT receiver
)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES "$<TARGET_RUNTIME_DLLS:${TARGET}>"
            TYPE BIN
            COMPONENT common
    )
    qt_generate_deploy_app_script(
            TARGET ${TARGET}
            OUTPUT_SCRIPT DEPLOY_SCRIPT
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=$<TARGET_FILE_DIR:${TARGET}>" -DQT_DEPLOY_BIN_DIR=.
            -DQT_DEPLOY_LIB_DIR=. -P "${DEPLOY_SCRIPT}"
            VERBATIM
    )
    install(
            SCRIPT ${DEPLOY_SCRIPT}
            COMPONENT common
    )
endif ()
