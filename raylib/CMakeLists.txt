cmake_minimum_required(VERSION 3.10)
project(pime-raylib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(enet REQUIRED libenet)
pkg_check_modules(raylib REQUIRED raylib)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DDISABLE_NPC ${enet_CFLAGS} ${raylib_CFLAGS}
)

add_subdirectory(../core core)

add_executable(pime_raylib
    main.cpp 
    playerUI.cpp
    net.cpp
    ../client-common/net.cpp
    ../client-common/game.cpp
    ../client-common/players.cpp

)

target_link_libraries(pime_raylib
    m 
    pime
    ${enet_LDFLAGS} ${raylib_LDFLAGS}
)

target_include_directories(pime_raylib PRIVATE  ${CMAKE_SOURCE_DIR} )
