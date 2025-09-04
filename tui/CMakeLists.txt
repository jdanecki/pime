cmake_minimum_required(VERSION 3.10)
project(pime-tui)

find_package(PkgConfig)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")

add_definitions(-DCORE_FOR_CLIENT -DDISABLE_NPC)

add_subdirectory(../core core)

add_executable(pime_tui
    main.cpp 
    ../SDL/networking.cpp 
    ../SDL/player_actions.cpp
    sdl_stub.cpp
    
    implementations/alchemistSDL.cpp
    implementations/BeingSDL.cpp
    implementations/playerSDL.cpp
)

include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/Rust)

# Add rust_example as a CMake target
ExternalProject_Add(
    the_game_networking
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build
    BINARY_DIR "${CMAKE_SOURCE_DIR}/../the_game_net/"
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    LOG_BUILD ON)

# Create dependency of Example on rust_example
add_dependencies(pime_tui the_game_networking)

target_link_directories(pime_tui PRIVATE ${CMAKE_SOURCE_DIR}/../the_game_net/target/debug/)
set(NETWORKING_LIB the_game_net)

target_link_libraries(pime_tui
    -lm 
    pime
    ${NETWORKING_LIB}
)

