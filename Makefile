all: configure serv serv-enet client tui-core tui-net tui-enet

include Makefile-cpp Makefile-rust

client: client-enet client-rust

configure: configure-common configure-enet configure-rust

configure-common:
	cd  core; ./generate_packet_types.sh
	cd tui; ./configure

configure-enet:
	cd SDL; ./configure USE_ENET
	cd server/enet; ./configure
	cd tui/enet; ./configure

configure-rust:
	cd SDL; ./configure

tui-core:
	make -C core/alchemist-tui/ -j $(nproc)


format:
	./format-all.sh
	
clean: client-clean tui-clean tui-net-clean 

client-clean:	
	make -C SDL/build clean
	cd the_game_net; cargo clean

tui-clean:	
	rm -f core/alchemist-tui/alchemist

tui-net-clean:
	rm -rf tui/build

tui-enet-clean:
	rm -rf tui/enet/build


