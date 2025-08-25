all: configure serv client tui tui-net

serv:
	cd server; 	cargo build

client:
	@make -C SDL/build -j $(nproc)

configure:
	cd  core; ./generate_packet_types.sh
	cd SDL; ./configure
	cd tui; ./configure

tui:
	@make -C core/alchemist-tui/ -j $(nproc)
	
tui-net:
	@make -C tui/build -j $(nproc)

clean:
	cd server; cargo clean
	rm -rf SDL/build

format:
	./format-all.sh
