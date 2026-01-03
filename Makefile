include config.txt

all: serv client client-tui godot-client
#   	tui-core

serv:
	+$(BUILD_TOOL) -C server/build

client: 
	+$(BUILD_TOOL) -C SDL/build

client-tui:
	+$(BUILD_TOOL) -C tui/build

tui-core:
	+$(BUILD_TOOL) -C core/alchemist-tui

clean: serv-clean client-clean tui-clean 

godot-client:
	cd godot/; scons platform=linux

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

