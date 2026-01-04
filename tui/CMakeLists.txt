cmake_minimum_required(VERSION 3.10)
project(pime-tui)

find_package(PkgConfig REQUIRED)
pkg_check_modules(enet REQUIRED libenet)

# FIXME remove SDL
pkg_check_modules(SDL REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#FIXME SDL
add_definitions(-DDISABLE_NPC ${enet_CFLAGS} ${SDL_CFLAGS} ${SDL2_IMAGE_CFLAGS} ${SDL2_TTF_CFLAGS} 
    ${SDL2_GFX_CFLAGS})

add_subdirectory(../core core)

add_executable(pime_tui
    main.cpp 
    net.cpp
    ../SDL/networking.cpp 
    ../SDL/player_actions.cpp
    sdl_stub.cpp
    
    implementations/alchemistSDL.cpp
    implementations/BeingSDL.cpp
    implementations/playerSDL.cpp
)

target_link_libraries(pime_tui
    -lm 
    pime
    enet
)

