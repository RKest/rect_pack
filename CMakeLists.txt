cmake_minimum_required(VERSION 3.28)

project(RectPack)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_STACKTRACE "Use stacktrace" OFF)

include(cmake/deps.cmake)
include(cmake/wasm.cmake)

add_library(rp_lib STATIC src/rect.cpp src/rp_c_api.cpp)

target_precompile_headers(rp_lib PRIVATE src/pch.h)
target_include_directories(rp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rp_lib PUBLIC 3rd_party)

macro(add_rp_executable name additional_libraries)
    add_executable(${name} exec/${name}.cpp)
    target_link_libraries(${name} PRIVATE rp_lib ${additional_libraries})
    target_precompile_headers(${name} REUSE_FROM rp_lib)
endmacro()

if (EMSCRIPTEN)
	add_rp_executable(rp_wasm "")
	set_target_properties(rp_wasm PROPERTIES LINK_FLAGS "-O3 -s MODULARIZE=1 -s EXPORT_ES6=1 --bind --emit-tsd rp.d.ts")
else()
	add_rp_executable(rp_native 3rd_party_raylib)
endif()

add_rp_executable(rp_test "")
