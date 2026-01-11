sdl_dep = dependency('sdl2', method: 'pkg-config', required: true)
sdl_image_dep = dependency('SDL2_image', method: 'pkg-config', required: true)
sdl_ttf_dep = dependency('SDL2_ttf', method: 'pkg-config', required: true)
sdl_mixer_dep = dependency('SDL2_mixer', method: 'pkg-config', required: true)
sdl_gfx_dep = dependency('SDL2_gfx', method: 'pkg-config', required: true)

sdl_deps = [sdl_dep, sdl_image_dep, sdl_ttf_dep, sdl_mixer_dep, sdl_gfx_dep, enet_dep]

sdl_sources = [
    'net.cpp',
    'main.cpp',
    'window.cpp',
    'music.cpp',
    'text.cpp',
    'texture.cpp',
    'backend.cpp',
    'inputs.cpp',
#    'npc.cpp',
    'playerUI.cpp',
    'palette.cpp'
]


inc = include_directories('.')

client_common_sdl = static_library('client_common_sdl', 
  client_common,
  #  cpp_args : ['-DDISABLE_NPC'],
  include_directories : inc,
  dependencies: [sdl_deps], 
)

executable('pime_SDL', 
  sources: sdl_sources,
  #  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [sdl_deps],
  link_args: shared_link_args,
  link_with: [core, client_common_sdl ],
  install: false,
)

custom_target(
  'textures_link',
  output: 'textures',
  command: ['ln', '-sf', meson.project_source_root() + '/assets/textures', '@OUTPUT@'],
  build_by_default: true
)

custom_target(
  'font_link',
  output: 'nerdfont.otf',
  command: ['ln', '-sf', meson.project_source_root() + '/assets/nerdfont.otf', '@OUTPUT@'],
  build_by_default: true,
)

