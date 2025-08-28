cmake_minimum_required(VERSION 3.10)
project(pime)

find_package(PkgConfig)
pkg_check_modules(SDL sdl2)
pkg_check_modules(SDL2_IMAGE SDL2_image)
pkg_check_modules(SDL2_TTF SDL2_ttf)
pkg_check_modules(SDL2_MIXER SDL2_mixer)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

add_definitions(-DCORE_FOR_CLIENT)

set(CORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core)
set(CORE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/core)

add_subdirectory(${CORE_SOURCE_DIR} ${CORE_BINARY_DIR})

set(MULTI_USER 1)

if(NOT MULTI_USER)
    set(NETWORK_STUB network_stub.cpp)
endif()

set(SHARED_SOURCES
    window.cpp music.cpp text.cpp
    texture.cpp main.cpp render.cpp menu.cpp
    networking.cpp dialog/dialog.cpp dialog/d_hotbar.cpp
    dialog/d_craft.cpp
    inputs.cpp player_actions.cpp npc.cpp
    implementations/alchemistSDL.cpp
    implementations/BeingSDL.cpp
    ${NETWORK_STUB}
)

set(SHARED_LIBRARIES
     SDL2 SDL2_image SDL2_ttf SDL2_mixer
    ${SDL_LDFLAGS} ${SDL2_MIXER_LDFLAGS}
    m 
    pime
)

add_executable(pime_SDL ${SHARED_SOURCES})

if (MULTI_USER)
    include(ExternalProject)
    set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/Rust)
    ExternalProject_Add(
        the_game_networking
        DOWNLOAD_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND cargo build
        BINARY_DIR "${CMAKE_SOURCE_DIR}/../the_game_net/"
        BUILD_ALWAYS 1
        INSTALL_COMMAND ""
        LOG_BUILD ON)

    add_dependencies(pime_SDL the_game_networking)

    target_link_directories(pime_SDL PRIVATE ${CMAKE_SOURCE_DIR}/../the_game_net/target/debug/)
    set(NETWORKING_LIB the_game_net)
endif()

target_link_libraries(pime_SDL
    ${SHARED_LIBRARIES} ${NETWORKING_LIB}
)

add_executable(pime_SDL_enet ${SHARED_SOURCES} ../tui/enet/net.cpp)
target_link_libraries(pime_SDL_enet
    ${SHARED_LIBRARIES} enet
)

