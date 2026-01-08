cmake_minimum_required(VERSION 3.10)
project(pime)

set(CMAKE_BUILD_TYPE Debug)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wno-switch -Wno-reorder")

add_library(pime STATIC
    world.cpp 
    player.cpp 
    tiles.cpp 
#    npc.cpp
    packet_types.cpp
    clan.cpp
    time.cpp
    alchemist/elements.cpp
    alchemist/names.cpp
    alchemist/el_list.cpp
    alchemist/npc_talk.cpp
    alchemist/properties.cpp
    alchemist/skills.cpp
    alchemist/serialization.cpp
    alchemist/object.cpp
)

add_definitions(${enet_CFLAGS} )
