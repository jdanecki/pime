cmake_minimum_required(VERSION 3.10)
project(pime)

set(CMAKE_BUILD_TYPE Debug)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

add_library(pime SHARED
    world.cpp 
    player.cpp 
    tiles.cpp 
    npc.cpp

    alchemist/elements.cpp
	alchemist/names.cpp
	alchemist/el_list.cpp
    alchemist/npc_talk.cpp
    alchemist/properties.cpp
	)

target_link_libraries(pime)
