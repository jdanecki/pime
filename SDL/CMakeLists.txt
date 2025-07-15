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

add_subdirectory(../core core)

set(MULTI_USER 1)

if(NOT MULTI_USER)
    set(NETWORK_STUB network_stub.cpp)
endif()

add_executable(pime_SDL
    window.cpp music.cpp text.cpp
    texture.cpp main.cpp render.cpp menu.cpp
    networking.cpp dialog/dialog.cpp dialog/d_hotbar.cpp
    inputs.cpp player_actions.cpp npc.cpp
    
    implementations/alchemistSDL.cpp
    implementations/BeingSDL.cpp

    ${NETWORK_STUB}
)

if (MULTI_USER)
    include(ExternalProject)

    # Set default ExternalProject root directory
    set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/Rust)

    # Add rust_example as a CMake target
    ExternalProject_Add(
        the_game_networking
        DOWNLOAD_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND cargo build
        BINARY_DIR "${CMAKE_SOURCE_DIR}/../the_game_net/"
        BUILD_ALWAYS 1
        INSTALL_COMMAND ""
        LOG_BUILD ON)

    # Create dependency of Example on rust_example
    add_dependencies(pime_SDL the_game_networking)

    target_link_directories(pime_SDL PRIVATE ${CMAKE_SOURCE_DIR}/../the_game_net/target/debug/)
    set(NETWORKING_LIB the_game_net)
endif()

target_link_libraries(pime_SDL
    SDL2 SDL2_image SDL2_ttf SDL2_mixer
    ${SDL_LDFLAGS} ${SDL2_MIXER_LDFLAGS}
    -lm 
    pime
    ${NETWORKING_LIB}
)

