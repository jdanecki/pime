cmake_minimum_required(VERSION 3.10)
project(server_enet)

find_package(PkgConfig)
pkg_check_modules(enet libenet)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

add_definitions(-DUSE_ENET -DSERVER_CODE ${enet_CFLAGS} )

set(SERVER_CPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-src)
set(SERVER_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/cpp-src)

add_subdirectory(${SERVER_CPP_SOURCE_DIR} ${SERVER_BINARY_DIR})

add_executable(server_enet
    server.cpp
)

target_link_libraries(server_enet
    server_cpp enet
)

