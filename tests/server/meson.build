ncurses_dep = dependency('ncurses', method: 'pkg-config', required: true)

server_ncurses_sources = [
  'ncurses.cpp',
  'functions.cpp'
]

executable('server_ncurses', 
  server_ncurses_sources,
  dependencies: [ ncurses_dep],
  install: false,
  
)


