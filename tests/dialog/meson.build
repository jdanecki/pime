
test_dialog_sdl_sources = [
    'test_dialog.cpp',
    'handle_events_sdl.cpp',
    '../../SDL/window.cpp',
    '../../SDL/backend.cpp',
    '../../SDL/text.cpp',
    '../../client-common/alchemist2d.cpp',
    '../../client-common/texture.cpp',
    '../../client-common/text.cpp',
    '../../dialog/dialog.cpp',
    '../../dialog/d_craft.cpp',
    '../../dialog/d_action.cpp'
]

test_dialog_raylib_sources = [
    'test_dialog.cpp',
    'handle_events_raylib.cpp',
    '../../raylib/window.cpp',
    '../../raylib/backend.cpp',
    '../../raylib/text.cpp',
    '../../client-common/alchemist2d.cpp',
    '../../client-common/texture.cpp',
    '../../client-common/text.cpp',
    '../../dialog/dialog.cpp',
    '../../dialog/d_craft.cpp',
    '../../dialog/d_action.cpp'
]

executable('test__dialog_sdl', 
  sources: test_dialog_sdl_sources,
  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [sdl_deps],
  include_directories :  include_directories('../../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_dialog_raylib', 
  sources: test_dialog_raylib_sources,
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

