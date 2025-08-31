cmake_minimum_required(VERSION 3.10)
project(pime)

set(CMAKE_BUILD_TYPE Debug)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

if(USE_ENET)
    set(PIME_TARGET_NAME pime_enet)
else()
    set(PIME_TARGET_NAME pime)
endif()

add_library(${PIME_TARGET_NAME} SHARED
    world.cpp 
    player.cpp 
    tiles.cpp 
    npc.cpp
    packet_types.cpp
    clan.cpp

    alchemist/elements.cpp
	alchemist/names.cpp
	alchemist/el_list.cpp
    alchemist/npc_talk.cpp
    alchemist/properties.cpp
    alchemist/skills.cpp
    alchemist/serialization.cpp
	)

if(USE_ENET)
    target_compile_definitions(${PIME_TARGET_NAME} PRIVATE USE_ENET)
endif()


