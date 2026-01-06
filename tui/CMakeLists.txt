cmake_minimum_required(VERSION 3.10)
project(pime-tui)

find_package(PkgConfig REQUIRED)
pkg_check_modules(enet REQUIRED libenet)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DDISABLE_NPC ${enet_CFLAGS}
)

add_subdirectory(../core core)

add_executable(pime_tui
    main.cpp 
    playerUI.cpp
    net.cpp
    ../client-common/net.cpp
    ../client-common/game.cpp
    ../client-common/players.cpp

)

target_link_libraries(pime_tui
    -lm 
    pime
    enet
)

target_include_directories(pime_tui PRIVATE  ${CMAKE_SOURCE_DIR} )
