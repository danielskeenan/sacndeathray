find_package(flatbuffers CONFIG REQUIRED)

set(MESSAGE_SOURCES
        Error.fbs
        Hello.fbs
        Message.fbs
        Ready.fbs
)

# Set where generated files go.
set(MESSAGE_CPP_HEADER_DIR "${PROJECT_BINARY_DIR}/sacndeathray/messages/cpp")
file(MAKE_DIRECTORY "${MESSAGE_CPP_HEADER_DIR}")
cmake_path(NATIVE_PATH MESSAGE_CPP_HEADER_DIR NORMALIZE MESSAGE_CPP_HEADER_DIR)

# Link with this target to use messages.
add_library(deathray_messages INTERFACE)
target_include_directories(deathray_messages INTERFACE "${MESSAGE_CPP_HEADER_DIR}/..")
target_link_libraries(deathray_messages INTERFACE flatbuffers::flatbuffers)

foreach (FBS ${MESSAGE_SOURCES})
    cmake_path(GET FBS STEM LAST_ONLY FBS_NAME)
    set(MESSAGE_CPP_HEADER "${MESSAGE_CPP_HEADER_DIR}/${FBS_NAME}.h")
    add_custom_command(
            OUTPUT "${MESSAGE_CPP_HEADER}"
            COMMAND flatbuffers::flatc --cpp -o "${MESSAGE_CPP_HEADER_DIR}/" --scoped-enums --force-defaults --filename-suffix "" --cpp-field-case-style lower "${FBS}"
            MAIN_DEPENDENCY "${FBS}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            VERBATIM
    )
    target_sources(deathray_messages PRIVATE "${MESSAGE_CPP_HEADER}")
endforeach ()
