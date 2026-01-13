project('pime-project', 'cpp',
  #default_options : ['buildtype=debugoptimized']
  default_options : [
    'buildtype=debug',
    'optimization=g', 
    'warning_level=0',
    #'warning_level=3',
    'b_pie=true',
    'b_lto=false'
  ],
  version : '0.7'
)
add_project_arguments(
  '-Wno-switch',
  '-Wno-reorder',
  '-Wno-unused-but-set-variable',
  '-DDISABLE_NPC',
  language : 'cpp'
)
enet_dep = dependency('libenet', method: 'pkg-config', required: true)
shared_link_args = ['-lm']

subdir('core')
subdir('client-common')
subdir('dialog')
subdir('menu')

client_common_2d= []
foreach s : client_common_sources
  client_common_2d += ['../client-common/' + s]
endforeach

foreach s : client_common_2d_sources
  client_common_2d += ['../client-common/' + s]
endforeach

foreach s : menu_sources
  client_common_2d += ['../menu/' + s]
endforeach

foreach s : dialog_sources
  client_common_2d += ['../dialog/' + s]
endforeach

subdir('SDL')
subdir('raylib')

subdir('server')
subdir('tui')
if get_option('tests')
  subdir('tests')
endif

custom_target(
  'godot-client',
  output : 'godot-client-build-done.txt',
  command : ['sh', '-c', 'scons -C godot platform=linux && touch $OUT'],
  capture : true,
  build_by_default : false
)

custom_target(
  'format',
  output : 'format',
  command : ['sh', '-c', 'cd @SOURCE_ROOT@ && @BUILD_ROOT@/format-all.sh && touch $OUT'],
  build_by_default : false
)

custom_target(
  'lines',
  output : 'lines',
  command : ['sh', '-c', 'cd @SOURCE_ROOT@ && echo find . -name "*.cpp" -o -name "*.h" -exec "wc -l {} \;" | awk "{s+=$$1} END { print s }" | tee lines'],
  build_by_default : false
)


