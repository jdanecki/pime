custom_target('build godot',
  command: ['scons', '-C', meson.project_source_root() + '/godot', 'platform=linux', 'debug_symbols=yes'],
  build_by_default: true,
  output: 'stamp-scons',
  console: true,
)
