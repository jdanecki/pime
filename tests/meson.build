sdl_ttf_dep = dependency('SDL2_ttf', method: 'pkg-config', required: true)

test_sdl_sources = [
    'test_sdl.cpp',
    '../SDL/window.cpp',
    '../SDL/backend.cpp',
    '../SDL/text.cpp',
    '../client-common/2d/alchemist2d.cpp',
    '../client-common/2d/texture.cpp',
    '../client-common/ui/console.cpp',
    '../client-common/ui/window.cpp'
]
executable('test_sdl', 
  sources: test_sdl_sources,
  dependencies: [sdl_deps, sdl_ttf_dep],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

test_raylib_sources = [
    'test_raylib.cpp',
    '../raylib/window.cpp',
    '../raylib/backend.cpp',
    '../raylib/text.cpp',
    '../client-common/2d/alchemist2d.cpp',
    '../client-common/2d/texture.cpp',
    '../client-common/ui/console.cpp',
    '../client-common/ui/window.cpp'
]
executable('test_raylib', 
  sources: test_raylib_sources,
  dependencies: [raylib_deps],
  include_directories :  include_directories('../raylib'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)


test_sdl_textures_sources = [
    'test_sdl_textures.cpp',
    '../SDL/window.cpp',
    '../SDL/text.cpp',
    '../SDL/backend.cpp',
    '../client-common/2d/texture.cpp',
    '../client-common/ui/console.cpp',
    '../client-common/ui/window.cpp'
]    
executable('test_sdl_textures', 
  sources: test_sdl_textures_sources,
  dependencies: [sdl_deps],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

test_sdl_textures1_sources = [
    'test_sdl_textures1.cpp',
    '../SDL/window.cpp',
    '../SDL/text.cpp',
    '../SDL/backend.cpp',
    '../client-common/2d/texture.cpp',
    '../client-common/ui/console.cpp',
    '../client-common/ui/window.cpp'
]    
executable('test_sdl_textures1', 
  sources: test_sdl_textures1_sources,
  dependencies: [sdl_deps],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
  link_with: [core],
  install: false,
)

test_lists_sources = [
    'test_lists.cpp',
    '../client-common/ui/console.cpp'
]
executable('test_lists', 
  sources: test_lists_sources,
  link_with: [core],
  install: false,
)

test_generator_sdl_sources = [
  'test_generator_sdl.cpp',
  '../server/generator/generator.cpp',
  '../core/alchemist/random_functions.cpp',
  '../SDL/text.cpp',
  '../SDL/palette.cpp',
  '../client-common/ui/console.cpp'
]
executable('test_generator_sdl', 
  sources: test_generator_sdl_sources,
  cpp_args : ['-DSHOW_PALETTE'],
  dependencies: [sdl_deps],
  include_directories :  include_directories('../SDL'),
  link_args: shared_link_args,
  install: false,
)

test_random_sources = [
    'test_random.cpp',
    '../core/alchemist/random_functions.cpp',
    '../client-common/ui/console.cpp'
]
executable('test_random', 
  sources: test_random_sources,
  link_with: [core],
  install: false,
)

test_serialization_sources = [
    'serialization.cpp',
    '../client-common/ui/console.cpp',
    'packets_server.cpp',
    'packets_client.cpp',
]
executable('test_serialization',
  sources: test_serialization_sources,
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
subdir('dialog')
subdir('menu')
subdir('server')
subdir('alchemist')

