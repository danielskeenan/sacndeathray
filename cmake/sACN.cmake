include(FetchContent)
FetchContent_Declare(sACN
        GIT_REPOSITORY "https://github.com/ETCLabs/sACN.git"
        GIT_TAG "v4.0.0.3"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(sACN)
