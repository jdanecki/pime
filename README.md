# The Game aka pime aka alchemist

This is a game made with sdl2 and C++. Port to Godot was restored.

![SDL version](game-gui-sdl.png)

## Dependencies installation

* [Installation instruction](install.md)

## Compilation configuration

* Set variables in config.txt

## Build

```
./configure
make -j `nproc`
```

## Run all together

```
./start.sh
```

## Build with meson

```
meson setup build .
meson compile -C build

```

## Run

* run server

In server/build run: 

```
./server_pime
```

* SDL client

In SDL directory run:

```
./build/pime_SDL
```

## This project uses nerd-font: https://github.com/ryanoasis/nerd-fonts

