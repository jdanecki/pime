ncurses_dep = dependency('ncurses', method: 'pkg-config', required: true)

server_sources = [
  'console.cpp',
  'server.cpp', 'generator/generator.cpp',
  'world_server.cpp', 'player_server.cpp',
  'networking.cpp', 'elements_server.cpp',
  'craft.cpp', 'craft_ing.cpp', 'craft_prod.cpp',

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

executable('server_pime', server_sources,
  dependencies: [enet_dep, ncurses_dep],
  install: false,
  link_with: [core],
  
)
