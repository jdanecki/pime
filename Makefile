all: configure serv client

serv:
	cd server; 	cargo build

client:
	@make -C SDL/build -j $(nproc)

configure:
	pushd core
	./generate_packet_types.sh
	popd
	pushd SDL
	./configure

tui:
	@make -C core/alchemist-tui/ -j $(nproc)
	
clean:
	cd server; cargo clean
	rm -rf SDL/build

format:
	./format-all.sh
