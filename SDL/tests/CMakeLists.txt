cmake_minimum_required(VERSION 3.10)
project(test_sdl)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wno-switch -Wno-reorder -Wno-unused-but-set-variable")
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "ccache")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "ccache")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(${SDL_CFLAGS} ${SDL2_IMAGE_CFLAGS}
    ${SDL2_TTF_CFLAGS} ${SDL2_MIXER_CFLAGS} ${SDL2_GFX_CFLAGS} )


add_executable(test_SDL 
    test_sdl.cpp
    ../window.cpp
    ../backend.cpp
    ../../core/time.cpp
    )

target_link_libraries(test_SDL
    ${SDL2_IMAGE_LDFLAGS} ${SDL2_TTF_LDFLAGS} ${SDL2_MIXER_LDFLAGS} ${SDL2_GFX_LDFLAGS} ${SDL_LDFLAGS}
    m
)

target_include_directories(test_SDL    PRIVATE  ${CMAKE_SOURCE_DIR}/.. )

