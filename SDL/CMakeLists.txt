cmake_minimum_required(VERSION 3.10)
project(pime)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)
pkg_check_modules(enet REQUIRED libenet)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wno-switch -Wno-reorder -Wno-unused-but-set-variable")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DDISABLE_NPC ${enet_CFLAGS} ${SDL_CFLAGS} ${SDL2_IMAGE_CFLAGS}
    ${SDL2_TTF_CFLAGS} ${SDL2_MIXER_CFLAGS} ${SDL2_GFX_CFLAGS} )

set(CORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core)
set(CORE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/core)

add_subdirectory(${CORE_SOURCE_DIR} ${CORE_BINARY_DIR})

add_executable(pime_SDL 
    net.cpp
    window.cpp music.cpp text.cpp
    texture.cpp main.cpp menu.cpp
    ../dialog/dialog.cpp 
    ../dialog/d_hotbar.cpp
    ../dialog/d_craft.cpp
    inputs.cpp 
    npc.cpp
    playerUI.cpp
    palette.cpp
    backend.cpp
    ../client-common/net.cpp
    ../client-common/game.cpp
    ../client-common/players.cpp
    ../client-common/player_actions.cpp 
    ../client-common/text.cpp 
    ../client-common/texture.cpp
    ../client-common/render.cpp
    ../client-common/alchemist2d.cpp
    ../client-common/being2d.cpp
)

target_link_libraries(pime_SDL
    pime 
    ${enet_LDFLAGS}
    ${SDL2_IMAGE_LDFLAGS} ${SDL2_TTF_LDFLAGS} ${SDL2_MIXER_LDFLAGS} ${SDL2_GFX_LDFLAGS} ${SDL_LDFLAGS}
    m
)

target_include_directories(pime_SDL    PRIVATE  ${CMAKE_SOURCE_DIR} )

