tui_sources = [
  'main.cpp',
  'playerUI.cpp',
  'net.cpp',
  'backend.cpp'
]
inc = include_directories('.')

client_common_tui_sources= []
foreach s : client_common_sources
  client_common_tui_sources += ['../client-common/' + s]
endforeach

client_common_tui = static_library('client_common_tui', 
  client_common_tui_sources,
  include_directories : inc,
  dependencies: [enet_dep], 
)

executable('pime_tui', 
  sources: [tui_sources],
  #  cpp_args : ['-DDISABLE_NPC'],
  dependencies: [enet_dep],
  #  link_args: shared_link_args,
  link_with: [core, client_common_tui ],
  install: false,
)


