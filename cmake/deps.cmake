include(FetchContent)

FetchContent_Declare(
        blackbird
        GIT_REPOSITORY https://github.com/RKest/blackbird
        GIT_TAG        main
)
FetchContent_MakeAvailable(blackbird)

FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/boost-ext/ut
        GIT_TAG        v2.0.1
)
FetchContent_MakeAvailable(ut)

FetchContent_Declare(
        ranges-v3
        GIT_REPOSITORY https://github.com/ericniebler/range-v3
        GIT_TAG        0.12.0
)
FetchContent_MakeAvailable(ranges-v3)

FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_linux_amd64.tar.gz
)
FetchContent_MakeAvailable(raylib)

add_library(3rd_party_raylib INTERFACE)
target_include_directories(3rd_party_raylib  INTERFACE ${raylib_SOURCE_DIR}/include)
target_link_directories(3rd_party_raylib INTERFACE ${raylib_SOURCE_DIR}/lib)
target_link_libraries(3rd_party_raylib INTERFACE raylib)

add_library(3rd_party INTERFACE)
target_include_directories(3rd_party INTERFACE 
        ${blackbird_SOURCE_DIR} 
        ${ut_SOURCE_DIR}/include 
        ${ranges-v3_SOURCE_DIR}/include
)

if (USE_STACKTRACE)
        target_compile_definitions(3rd_party INTERFACE USE_STACKTRACE)
        target_link_libraries(3rd_party INTERFACE stdc++_libbacktrace)
endif()
