test_sdl_sources = [
    'test_sdl.cpp',
    '../SDL/window.cpp',
    '../SDL/backend.cpp',
    '../SDL/text.cpp',
    '../client-common/alchemist2d.cpp',
    '../client-common/texture.cpp',
    '../client-common/text.cpp'
]

test_raylib_sources = [
    'test_raylib.cpp',
    '../raylib/window.cpp',
    '../raylib/backend.cpp',
    '../raylib/text.cpp',
    '../client-common/alchemist2d.cpp',
    '../client-common/texture.cpp',
    '../client-common/text.cpp'
]


test_sdl_textures_sources = [
    'test_sdl_textures.cpp',
    '../SDL/window.cpp',
    '../SDL/text.cpp',
    '../SDL/backend.cpp',
    '../client-common/texture.cpp',
    '../client-common/text.cpp'
]    

test_lists_sources = [
    'test_lists.cpp',
    '../client-common/text.cpp'
]

test_generator_sdl_sources = [
  'test_generator_sdl.cpp',
  '../server/generator/generator.cpp',
  '../SDL/text.cpp',
  '../SDL/palette.cpp',
  '../client-common/text.cpp'
]


executable('test_sdl', 
  sources: test_sdl_sources,
  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [sdl_deps],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_raylib', 
  sources: test_raylib_sources,
  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [raylib_deps],
  include_directories :  include_directories('../raylib'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_sdl_textures', 
  sources: test_sdl_textures_sources,
  dependencies: [sdl_deps],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

executable('test_lists', 
  sources: test_lists_sources,
  link_with: [core],
  install: false,
)

executable('test_generator_sdl', 
  sources: test_generator_sdl_sources,
  cpp_args : ['-DSHOW_PALETTE'],
  dependencies: [sdl_deps],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
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

subdir('dialog')
subdir('menu')
subdir('server')
