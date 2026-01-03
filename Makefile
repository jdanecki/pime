all: serv client client-tui
#   	tui-core

serv:
	ninja -C server/build

client: 
	ninja -C SDL/build

client-tui:
	ninja -C tui/build

tui-core:
	ninja -C core/alchemist-tui

clean: serv-clean client-clean tui-clean tui-core-clean

serv-clean:
	ninja -C server/build clean

client-clean:
	ninja -C SDL/build clean

tui-clean:
	ninja -C tui/build clean

tui-core-clean:	
	ninja -C core/alchemist-tui clean

format:
	./format-all.sh

distclean:
	rm -rf SDL/build
	rm -rf server/build
	rm -rf tui/build

lines:
	find . -name *.cpp -o -name *.h  -exec wc -l {} \; | awk '{s+=$$1} END { print s} '

