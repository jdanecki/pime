project('pime-project', 'cpp',
  #default_options : ['buildtype=debugoptimized']
  default_options : [
    'buildtype=debug',
    'optimization=0',
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
  language : 'cpp'
)
enet_dep = dependency('libenet', method: 'pkg-config', required: true)
shared_link_args = ['-lm']

subdir('core')
subdir('client-common')
subdir('net')

client_common_2d= []
foreach s : client_common_sources
  client_common_2d += ['../client-common/' + s]
endforeach

foreach s : client_common_2d_sources
  client_common_2d += ['../client-common/' + s]
endforeach

client_net= []
foreach s : client_net_sources
  client_net += ['../net/' + s]
endforeach

subdir('ogl')
subdir('SDL')
subdir('raylib')
subdir('godot')

subdir('server')
subdir('tui')
if get_option('tests')
  subdir('tests')
endif

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


