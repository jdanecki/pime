all: serv client 
#tui tui-core

serv:
	make -C server/enet/build

client: 
	make -C SDL/build pime_SDL

tui:
	make -C tui/enet/build

tui-core:
	make -C core/alchemist-tui

clean: serv-clean client-clean tui-clean tui-core-clean

serv-clean:	
	make -C server/enet/build clean

client-clean:
	make -C SDL/build clean

tui-clean:
	make -C tui/enet/build clean

tui-core-clean:	
	make -C core/alchemist-tui clean

format:
	./format-all.sh

lines:
	find . -name *.cpp -o -name *.h  -exec wc -l {} \; | awk '{s+=$$1} END { print s} '

