# The Game aka pime aka alchemist

## Build

### All versions
To build server and client run:

```
make
```

### Enet version

```
make configure enet
make enet
```

## Run

### Enet version

* run server

In server/enet/build run: 

```
./server_enet
```

* SDL client

In SDL directory run:

```
./build/pime_SDL_enet
```

### Rust version 

* run server

In server directory run: 

```
cargo run
```

* SDL Client

In SDL directory run: 

```
./run_local.sh
```

