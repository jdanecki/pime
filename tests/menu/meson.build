test_menu_sdl_sources = [
    'test_menu.cpp',
    'handle_events_sdl.cpp',
    '../../SDL/window.cpp',
    '../../SDL/backend.cpp',
    '../../SDL/text.cpp',
    '../../client-common/players.cpp',
    '../../client-common/player_actions.cpp',
    '../../client-common/texture.cpp',
    '../../client-common/text.cpp',
    '../../menu/menu.cpp',
]

test_menu_raylib_sources = [
    'test_menu.cpp',
    'handle_events_raylib.cpp',
    '../../raylib/window.cpp',
    '../../raylib/backend.cpp',
    '../../raylib/text.cpp',
    '../../client-common/players.cpp',
    '../../client-common/player_actions.cpp',
    '../../client-common/texture.cpp',
    '../../client-common/text.cpp',
    '../../menu/menu.cpp'
]

executable('test_menu_sdl', 
  sources: test_menu_sdl_sources,
  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [sdl_deps],
  include_directories :  include_directories('../../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_menu_raylib', 
  sources: test_menu_raylib_sources,
  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [raylib_deps],
  include_directories :  include_directories('../../raylib'),
  link_args: shared_link_args,
  link_with: [core],
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

