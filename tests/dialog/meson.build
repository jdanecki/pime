
test_dialog_sdl_sources = [
    'test_dialog.cpp',
    'handle_events_sdl.cpp',
    '../../SDL/window.cpp',
    '../../SDL/backend.cpp',
    '../../SDL/text.cpp',
    '../../client-common/2d/alchemist2d.cpp',
    '../../client-common/2d/texture.cpp',
    '../../client-common/ui/console.cpp',
    '../../client-common/ui/window.cpp',
    '../../client-common/ui/dialog/dialog.cpp',
    '../../client-common/ui/dialog/d_craft.cpp',
    '../../client-common/ui/dialog/d_action.cpp'
]

test_dialog_raylib_sources = [
    'test_dialog.cpp',
    'handle_events_raylib.cpp',
    '../../raylib/window.cpp',
    '../../raylib/backend.cpp',
    '../../raylib/text.cpp',
    '../../client-common/2d/alchemist2d.cpp',
    '../../client-common/2d/texture.cpp',
    '../../client-common/ui/console.cpp',
    '../../client-common/ui/window.cpp',
    '../../client-common/ui/dialog/dialog.cpp',
    '../../client-common/ui/dialog/d_craft.cpp',
    '../../client-common/ui/dialog/d_action.cpp'
]

executable('test_dialog_sdl', 
  sources: test_dialog_sdl_sources,
  dependencies: [sdl_deps],
  include_directories :  include_directories('../../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_dialog_raylib', 
  sources: test_dialog_raylib_sources,
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
custom_target(
  'font_png_link',
  output: 'font.png',
  command: ['ln', '-sf', meson.project_source_root() + '/assets/font-special.png', '@OUTPUT@'],
  build_by_default: true,
)
