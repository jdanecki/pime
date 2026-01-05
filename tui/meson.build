enet_dep = dependency('libenet', method: 'pkg-config', required: true)

tui_src = [
  'main.cpp',
  'playerUI.cpp',
  'net.cpp',
  '../client-common/net.cpp',
  '../client-common/game.cpp'
]

executable('pime_tui', tui_src,
  dependencies: [enet_dep],
  link_args: ['-lm'],
  install: false,
  cpp_args: ['-Wall', '-Wno-switch', '-DDISABLE_NPC']
)
