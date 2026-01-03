all: configure serv client tui tui-core

configure:
	cd  core; ./generate_packet_types.sh
	cd tui; ./configure
	cd SDL; ./configure USE_ENET
	cd server/enet; ./configure
	cd tui/enet; ./configure

serv:
	make -C server/enet/build -j $(shell nproc)

client: 
	make -C SDL/build -j $(shell nproc) pime_SDL_enet

tui:
	make -C tui/enet/build -j $(shell nproc)

tui-core:
	make -C core/alchemist-tui/ -j $(shell nproc)

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

