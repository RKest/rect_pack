
if (CMAKE_TOOLCHAIN_FILE MATCHES "Emscripten.cmake$")
        set(EMSCRIPTEN ON)
        message(STATUS "Compiling for Emscripten")
endif()
