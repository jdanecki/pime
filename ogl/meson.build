sdl3_dep = dependency('sdl3', required:true)
gl_dep = dependency('gl', required:true)

executable(
  'pime_ogl',
  ['main.cpp', '../client-common/net.cpp', '../client-common/packets.cpp', 'net.cpp'],
  dependencies: [sdl3_dep, gl_dep, enet_dep],
  link_args: shared_link_args,
  link_with: [core],
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
