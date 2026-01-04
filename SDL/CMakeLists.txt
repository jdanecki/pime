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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch -Wno-reorder -Wno-unused-but-set-variable")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(${enet_CFLAGS} )

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
    implementations/playerSDL.cpp
    palette.cpp

    ${NETWORK_STUB}
)

set(SHARED_LIBRARIES
     SDL2 SDL2_image SDL2_ttf SDL2_mixer SDL2_gfx
    ${SDL_LDFLAGS} ${SDL2_MIXER_LDFLAGS}
    m
)

add_executable(pime_SDL ${SHARED_SOURCES} ../tui/net.cpp)
target_link_libraries(pime_SDL
    pime ${SHARED_LIBRARIES} enet
)

