ncurses_dep = dependency('ncurses', method: 'pkg-config', required: true)

server_ncurses_sources = [
  'ncurses.cpp',
  'functions.cpp'
]

tests_server_sources =[
  'elements_server.cpp',
  'craft_ing.cpp',
  'craft_prod.cpp',

  'tools/axe_blade.cpp', 'tools/axe_handle.cpp', 'tools/axe.cpp',
  'tools/pickaxe_blade.cpp', 'tools/pickaxe_handle.cpp', 'tools/pickaxe.cpp',
  'tools/hoe_blade.cpp', 'tools/hoe_handle.cpp', 'tools/hoe.cpp',
  'tools/knife_blade.cpp', 'tools/knife_handle.cpp', 'tools/knife.cpp',
  'tools/wall.cpp', 'tools/hut.cpp', 'tools/tent.cpp',
  'tools/meat.cpp', 'tools/roasted_meat.cpp',
  'tools/tinder.cpp', 'tools/log.cpp', 'tools/stick.cpp', 'tools/fire.cpp',
  'tools/fruit.cpp', 'tools/fruit_salad.cpp',
  'tools/seed.cpp', 'tools/seedling.cpp',
  'places/places.cpp'
]

server_elements_sources = [
  'elements.cpp',
  ]

foreach s : tests_server_sources
  server_elements_sources += ['../../server/' + s]
endforeach

executable('server_ncurses', 
  server_ncurses_sources,
  dependencies: [ ncurses_dep],
  install: false,
)


executable('server_elements', 
  server_elements_sources,
  install: false,
  link_with: [core],
)
