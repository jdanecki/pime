sdl_dep = dependency('sdl2', method: 'pkg-config', required: true)
sdl_image_dep = dependency('SDL2_image', method: 'pkg-config', required: true)
sdl_ttf_dep = dependency('SDL2_ttf', method: 'pkg-config', required: true)
sdl_mixer_dep = dependency('SDL2_mixer', method: 'pkg-config', required: true)
sdl_gfx_dep = dependency('SDL2_gfx', method: 'pkg-config', required: true)
enet_dep = dependency('libenet', method: 'pkg-config', required: true)

multi_user = get_option('multi_user')

shared_sources = [
  'window.cpp', 'music.cpp', 'text.cpp',
  'texture.cpp', 'main.cpp', 'render.cpp', 'menu.cpp',
  'networking.cpp', 'dialog/dialog.cpp', 'dialog/d_hotbar.cpp',
  'dialog/d_craft.cpp', 'inputs.cpp', 'player_actions.cpp', 'npc.cpp',
  'implementations/alchemistSDL.cpp', 'implementations/BeingSDL.cpp',
  'implementations/playerSDL.cpp', 'palette.cpp'
]

if not multi_user
  shared_sources += ['network_stub.cpp']
endif

shared_sources += ['../tui/net.cpp']

sdl_deps = [sdl_dep, sdl_image_dep, sdl_ttf_dep, sdl_mixer_dep, sdl_gfx_dep, enet_dep]

shared_link_args = ['-lm']

executable('pime_SDL', shared_sources,
  dependencies: [pime_dep, sdl_deps],
  link_args: shared_link_args,
  install: false,
  cpp_args: ['-Wall', '-Wno-switch', '-Wno-reorder', '-Wno-unused-but-set-variable']
)
