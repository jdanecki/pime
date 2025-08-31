cmake_minimum_required(VERSION 3.10)
project(pime_tui_enet)

find_package(PkgConfig)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

add_definitions(-DCORE_FOR_CLIENT -DUSE_ENET)
set(USE_ENET ON)
add_subdirectory(../../core core)

add_executable(pime_tui_enet
    ../main.cpp 
    net.cpp
    ../../SDL/networking.cpp 
    ../../SDL/player_actions.cpp
  #  ../../SDL/npc.cpp
    ../sdl_stub.cpp
    
    ../implementations/alchemistSDL.cpp
    ../implementations/BeingSDL.cpp
    ../implementations/playerSDL.cpp

)

target_link_libraries(pime_tui_enet
    m
    pime_enet
    enet
)

