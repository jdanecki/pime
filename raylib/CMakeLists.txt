cmake_minimum_required(VERSION 3.10)
project(pime-raylib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(enet REQUIRED libenet)
pkg_check_modules(raylib REQUIRED raylib)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DDISABLE_NPC ${enet_CFLAGS} ${raylib_CFLAGS}
)

add_subdirectory(../core core)

add_executable(pime_raylib
    net.cpp
    main.cpp 
    window.cpp music.cpp text.cpp
    texture.cpp
    backend.cpp
    inputs.cpp
#    npc.cpp
    playerUI.cpp

    ../menu/menu.cpp
    
    ../dialog/dialog.cpp 
    ../dialog/d_hotbar.cpp
    ../dialog/d_craft.cpp

    ../client-common/net.cpp
    ../client-common/game.cpp
    ../client-common/players.cpp
    ../client-common/player_actions.cpp 
    ../client-common/text.cpp
    ../client-common/texture.cpp
    ../client-common/render.cpp
    ../client-common/alchemist2d.cpp
    ../client-common/being2d.cpp
    ../client-common/inputs.cpp
)

target_link_libraries(pime_raylib
    m 
    pime
    ${enet_LDFLAGS} ${raylib_LDFLAGS}
)

target_include_directories(pime_raylib PRIVATE  ${CMAKE_SOURCE_DIR} )
