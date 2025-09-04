all: configure serv serv-enet client tui-core tui-net tui-enet

clean: client-clean serv-clean serv-enet-clean tui-clean tui-net-clean tui-enet-clean

enet: serv-enet client-enet tui-enet

client: client-enet client-rust

serv:
	cd server; 	cargo build

serv-clean:
	cd server; cargo clean

serv-enet:
	make -C server/enet/build -j $(nproc)

serv-enet-clean:	
	rm -rf server/enet/build

client-enet:
	make -C SDL/build -j $(nproc) pime_SDL_enet

client-rust:
	make -C SDL/build -j $(nproc) pime_SDL

client-clean:	
	rm -rf SDL/build
	cd the_game_net; cargo clean

configure:
	cd  core; ./generate_packet_types.sh
	cd SDL; ./configure
	cd tui; ./configure
	cd server/enet; ./configure
	cd tui/enet; ./configure

tui-core:
	make -C core/alchemist-tui/ -j $(nproc)
	
tui-clean:	
	rm -f core/alchemist-tui/alchemist

tui-net:
	make -C tui/build -j $(nproc)

tui-net-clean:
	rm -rf tui/build

tui-enet:
	make -C tui/enet/build -j $(nproc)

tui-enet-clean:
	rm -rf tui/enet/build

format:
	./format-all.sh
