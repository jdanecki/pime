run_target('generate_packet_types',
  command: ['bash', '-lc', 'cd @SOURCE_DIR@ && ./generate_packet_types.sh']
)
subdir('alchemist')

src = [
  'world.cpp',
  'player.cpp',
  'tiles.cpp',
  'npc.cpp',
  'packet_types.cpp',
  'clan.cpp',
  'alchemist/elements.cpp',
  'alchemist/names.cpp',
  'alchemist/el_list.cpp',
  'alchemist/npc_talk.cpp',
  'alchemist/properties.cpp',
  'alchemist/skills.cpp',
  'alchemist/serialization.cpp',
  'alchemist/object.cpp'
]

# FIXME:
# enet_cflags = get_option('enet_cflags')
# if enet_cflags != ''
#   add_project_arguments(enet_cflags.split(), language: 'cpp')
# endif

pime = static_library('pime', src,
  install: false,
  cpp_args: ['-Wall', '-Wno-switch', '-Wno-reorder']
)

pime_dep = declare_dependency(
  link_with: pime
)
