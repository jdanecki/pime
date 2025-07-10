cmake_minimum_required(VERSION 3.10)
project(pime)

set(CMAKE_BUILD_TYPE Debug)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

add_subdirectory(../../core core)

add_library(server_cpp SHARED
    world_server.cpp player_server.cpp
    networking.cpp elements_server.cpp
    craft.cpp
    craft_ing.cpp
    craft_prod.cpp

    tools/axe_blade.cpp
    tools/axe_handle.cpp
    tools/axe.cpp

    tools/pickaxe_blade.cpp
    tools/pickaxe_handle.cpp
    tools/pickaxe.cpp

    tools/knife_blade.cpp
    tools/knife_handle.cpp
    tools/knife.cpp

    tools/wall.cpp
    tools/hut.cpp
)

target_link_libraries(server_cpp pime)
