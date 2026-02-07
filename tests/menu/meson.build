test_menu_sdl_sources = [
    'test_menu.cpp',
    'handle_events_sdl.cpp',
    '../../SDL/window.cpp',
    '../../SDL/backend.cpp',
    '../../SDL/text.cpp',
    '../../client-common/players.cpp',
    '../../client-common/2d/player_actions.cpp',
    '../../client-common/2d/texture.cpp',
    '../../client-common/ui/console.cpp',
    '../../client-common/ui/window.cpp',
    '../../client-common/ui/menu/menu.cpp',
    '../../client-common/ui/menu/menu_npc.cpp',
    '../../client-common/ui/menu/menu_action.cpp',
    '../../client-common/ui/menu/menu_debug.cpp',
    '../../client-common/ui/menu/menu_help.cpp',
    '../../client-common/ui/menu/menu_inventory.cpp',
    '../../client-common/ui/menu/menu_knowledge.cpp',
]

test_menu_raylib_sources = [
    'test_menu.cpp',
    'handle_events_raylib.cpp',
    '../../raylib/window.cpp',
    '../../raylib/backend.cpp',
    '../../raylib/text.cpp',
    '../../client-common/players.cpp',
    '../../client-common/2d/player_actions.cpp',
    '../../client-common/2d/texture.cpp',
    '../../client-common/ui/console.cpp',
    '../../client-common/ui/window.cpp',
    '../../client-common/ui/menu/menu.cpp',
    '../../client-common/ui/menu/menu_npc.cpp',
    '../../client-common/ui/menu/menu_action.cpp',
    '../../client-common/ui/menu/menu_debug.cpp',
    '../../client-common/ui/menu/menu_help.cpp',
    '../../client-common/ui/menu/menu_inventory.cpp',
    '../../client-common/ui/menu/menu_knowledge.cpp',
]

executable('test_menu_sdl', 
  sources: test_menu_sdl_sources,
  dependencies: [sdl_deps],
  include_directories :  include_directories('../../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_menu_raylib', 
  sources: test_menu_raylib_sources,
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
  'font_otf_link',
  output: 'nerdfont.otf',
  command: ['ln', '-sf', meson.project_source_root() + '/assets/nerdfont.otf', '@OUTPUT@'],
  build_by_default: true,
)

custom_target(
  'font_png_link',
  output: 'font.png',
  command: ['ln', '-sf', meson.project_source_root() + '/assets/font-special.png', '@OUTPUT@'],
  build_by_default: true,
)