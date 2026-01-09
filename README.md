# The Game aka pime aka alchemist

This game is currently under development using
* C++
* [SDL2](https://www.libsdl.org)
* [Raylib](https://github.com/raysan5/raylib.git)
* [Godot](https://github.com/godotengine/godot.git)

![SDL version](pime-sdl.png)
![Raylib version](pime-raylib.png)

## Dependencies installation

* [Installation instruction](install.md)

## Compilation configuration

* Set variables in config.txt

## Build

```
./configure
make -j `nproc`
```

## Run server and SDL client together

```
./start.sh
```

## Build with meson

```
meson setup build .
meson compile -C build

```

## Run

* Server

In server/build directory run: 

```
./server_pime
```

* SDL client

In SDL directory run:

```
./run_local.sh
```

* Raylib client

In raylib directory run:

```
./run_local.sh
```

## This project uses nerd-font: https://github.com/ryanoasis/nerd-fonts

