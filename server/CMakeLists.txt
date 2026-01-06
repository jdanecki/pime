cmake_minimum_required(VERSION 3.10)
project(server_pime)

find_package(PkgConfig REQUIRED)
pkg_check_modules(enet REQUIRED libenet)
pkg_check_modules(ncurses REQUIRED ncurses)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DSERVER_CODE -DUSE_NCURSES ${enet_CFLAGS} ${ncurses_CFLAGS} )

add_subdirectory(../core core)

add_executable(server_pime
    console.cpp
    server.cpp generator/generator.cpp
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

    tools/hoe_blade.cpp
    tools/hoe_handle.cpp
    tools/hoe.cpp

    tools/knife_blade.cpp
    tools/knife_handle.cpp
    tools/knife.cpp

    tools/wall.cpp
    tools/hut.cpp
    tools/tent.cpp

    tools/meat.cpp
    tools/roasted_meat.cpp

    tools/tinder.cpp
    tools/log.cpp
    tools/stick.cpp
    tools/fire.cpp

    tools/fruit.cpp
    tools/fruit_salad.cpp

    tools/seed.cpp
    tools/seedling.cpp

    places/places.cpp
)

target_link_libraries(server_pime
    pime ${enet_LDFLAGS}  ${ncurses_LDFLAGS}
)



