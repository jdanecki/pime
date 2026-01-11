raylib_dep = dependency('raylib', method: 'pkg-config', required: true)

raylib_deps = [raylib_dep, enet_dep]

raylib_sources = [
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
]

inc = include_directories('.')

client_common_raylib = static_library('client_common_raylib', 
  client_common_2d,
  #  cpp_args : ['-DDISABLE_NPC'],
  include_directories : inc,
  dependencies: [raylib_deps], 
)

executable('pime_raylib', 
  sources: raylib_sources,
  #  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [raylib_deps],
  link_args: shared_link_args,
  link_with: [core, client_common_raylib ],
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

