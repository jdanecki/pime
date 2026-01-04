project('alchemist-project', 'cpp',
  default_options : ['buildtype=debugoptimized'])


subdir('core')
# subdir('tui')
subdir('SDL')
subdir('server')

godot_client = custom_target(
  'godot-client',
  output : 'godot-client-build-done.txt',
  command : ['sh', '-c', 'scons -C godot platform=linux && touch $OUT'],
  capture : true,
  build_by_default : false
)

format_all = custom_target(
  'format',
  output : 'format',
  command : ['sh', '-c', 'cd @SOURCE_ROOT@ && @BUILD_ROOT@/format-all.sh && touch $OUT'],
  build_by_default : false
)

lines = custom_target(
  'lines',
  output : 'lines',
  command : ['sh', '-c', 'cd @SOURCE_ROOT@ && echo find . -name "*.cpp" -o -name "*.h" -exec "wc -l {} \;" | awk "{s+=$$1} END { print s }" | tee lines'],
  build_by_default : false
)

clean_generated = custom_target(
  'clean_generated',
  output : 'clean_generated',
  command : ['sh', '-c', 'cd @SOURCE_ROOT@ && ./clean.sh']
)
