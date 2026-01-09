cmake_minimum_required(VERSION 3.10)
project(test_raylib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(raylib REQUIRED raylib)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wno-switch -Wno-reorder -Wno-unused-but-set-variable")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(${raylib_CFLAGS} )

add_subdirectory(../../core core)

add_executable(test_raylib
    test_raylib.cpp
    ../window.cpp
    ../backend.cpp
    ../../client-common/alchemist2d.cpp
    ../../client-common/texture.cpp
    )

target_link_libraries(test_raylib
    ${raylib_LDFLAGS}
    m
    pime
)

target_include_directories(test_raylib    PRIVATE  ${CMAKE_SOURCE_DIR}/.. )

