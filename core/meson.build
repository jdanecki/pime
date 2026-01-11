run_command(['bash', meson.global_source_root() + '/core/generate_packet_types.sh'], check: true)
subdir('alchemist')

core_sources = [
  'world.cpp',
  'player.cpp',
  'tiles.cpp',
  'npc.cpp',
  'packet_types.cpp',
  'clan.cpp',
  'time.cpp',
]

core_alchemist_sources = []
foreach s : alchemist_sources
  core_alchemist_sources += ['alchemist/' + s]
endforeach

core = static_library('core', 
  [core_sources, core_alchemist_sources]
  )


