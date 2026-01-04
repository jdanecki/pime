enet_dep = dependency('libenet', method: 'pkg-config', required: true)

run_target('link implementations from SDL',
  command: ['bash', '-lc', 'cd @SOURCE_DIR@ && ln -sf @SOURCE_ROOT@/SDL/implementations @SOURCE_DIR@/implementations']
)

tui_src = [
  'main.cpp',
  'net.cpp',
  '../SDL/networking.cpp',
  '../SDL/player_actions.cpp',
  'sdl_stub.cpp',
  'implementations/alchemistSDL.cpp',
  'implementations/BeingSDL.cpp',
  'implementations/playerSDL.cpp'
]

executable('pime_tui', tui_src,
  dependencies: [enet_dep],
  link_args: ['-lm'],
  install: false,
  cpp_args: ['-Wall', '-Wno-switch', '-DDISABLE_NPC']
)
