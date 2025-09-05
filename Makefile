all: configure serv client

include Makefile-enet Makefile-rust

serv: serv-rust serv-enet

client: client-enet client-rust tui-core tui-enet tui-rust

configure: configure-enet configure-rust

configure-common:
	cd  core; ./generate_packet_types.sh
	cd tui; ./configure
	
clean: client-clean serv-clean

client-clean: tui-clean tui-rust-clean tui-enet-clean
	make -C SDL/build clean
	cd the_game_net; cargo clean

serv-clean: serv-rust-clean serv-enet-clean

tui-core:
	make -C core/alchemist-tui/ -j $(nproc)

tui-clean:	
	make -C core/alchemist-tui clean

format:
	./format-all.sh
