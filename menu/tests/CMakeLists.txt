cmake_minimum_required(VERSION 3.10)
project(test_menu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDISABLE_NPC")

set(CLIENT_FILES
    ../../client-common/text.cpp
    ../../client-common/players.cpp
    ../../client-common/player_actions.cpp
    ../../client-common/input.cpp
)
set(MENU_FILES
        ../menu.cpp
)
#set(DIALOG_FILES
#    ../../dialog/dialog.cpp
#    ../../dialog/d_craft.cpp
#    ../../dialog/d_action.cpp
#)

set(SOURCES
    test_menu.cpp
    ${DIALOG_FILES}
    ${MENU_FILES}
    ${CLIENT_FILES}
)

set(SDL_FILES
    ../../SDL/window.cpp
    ../../SDL/text.cpp
    ../../SDL/backend.cpp
    handle_events_sdl.cpp
)
set(RAYLIB_FILES
    ../../raylib/window.cpp
    ../../raylib/text.cpp
    ../../raylib/backend.cpp
    handle_events_raylib.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_image SDL2_ttf SDL2_gfx SDL2_mixer)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/textures
    COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/../../assets/textures ${CMAKE_CURRENT_BINARY_DIR}/textures
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../assets/textures
)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nerdfont.otf
    COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/../../assets/nerdfont.otf ${CMAKE_CURRENT_BINARY_DIR}/nerdfont.otf
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../assets/nerdfont.otf
)

add_custom_target(link_assets ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR}/nerdfont.otf
)

add_executable(test_menu_sdl ${SOURCES} ${SDL_FILES})
target_include_directories(test_menu_sdl PRIVATE ${SDL2_INCLUDE_DIRS} ../../SDL)
target_link_libraries(test_menu_sdl ${SDL2_LIBRARIES} pime)
target_link_directories(test_menu_sdl PRIVATE ${SDL2_LIBRARY_DIRS})

pkg_check_modules(RAYLIB REQUIRED raylib)

add_executable(test_menu_raylib ${SOURCES} ${RAYLIB_FILES})
target_include_directories(test_menu_raylib PRIVATE ${RAYLIB_INCLUDE_DIRS} ../../raylib)
target_link_libraries(test_menu_raylib ${RAYLIB_LIBRARIES} pime)
target_link_directories(test_menu_raylib PRIVATE ${RAYLIB_LIBRARY_DIRS})

set(CORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../core)
set(CORE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/core)

add_subdirectory(${CORE_SOURCE_DIR} ${CORE_BINARY_DIR})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/core)
