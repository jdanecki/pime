include config.txt

all: 2d 3d
#   	tui-core

2d: serv client client-tui

3d: serv godot-client

serv:
	+$(BUILD_TOOL) -C server/build

client: 
	+$(BUILD_TOOL) -C SDL/build

client-tui:
	+$(BUILD_TOOL) -C tui/build

tui-core:
	+$(BUILD_TOOL) -C core/alchemist-tui

clean: 2d-clean 3d-clean
	
2d-clean: serv-clean client-clean tui-clean 

3d-clean: serv-clean godot-clean

godot-client:
	+scons -C godot platform=linux

serv-clean:
	+$(BUILD_TOOL) -C server/build clean

client-clean:
	+$(BUILD_TOOL) -C SDL/build clean

tui-clean:
	+$(BUILD_TOOL) -C tui/build clean

tui-core-clean:	
	+$(BUILD_TOOL) -C core/alchemist-tui clean

format:
	./format-all.sh

distclean:
	rm -rf SDL/build
	rm -rf server/build
	rm -rf tui/build

lines:
	find . -name *.cpp -o -name *.h  -exec wc -l {} \; | awk '{s+=$$1} END { print s} '

tests:
	make -C SDL/dialog
	make -C server/tests
	make -C core/alchemist/tests

rebuild: distclean
	./configure
	+$(BUILD_TOOL) 2d

