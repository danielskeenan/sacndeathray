include(FetchContent)
FetchContent_Declare(sACN
        GIT_REPOSITORY "https://github.com/ETCLabs/sACN.git"
        GIT_TAG "v4.0.0.3"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(sACN)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # ETCPal's bundled SHA1 library triggers this error. We don't use that functionality in this program anyway.
    target_link_options(EtcPal PUBLIC "-Wno-stringop-overread")
endif ()
