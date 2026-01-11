## Install dependencies under Ubuntu

```
sudo apt install cmake pkg-config libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev  libsdl2-mixer-dev libsdl2-gfx-dev libenet-dev
```

## Install dependencies under Fedora

```
sudo dnf install cmake pkg-config SDL2-devel SDL2_mixer-devel SDL2_image-devel SDL2_ttf-devel SDL2_gfx-devel enet-devel
```

## Install dependencies under Arch

```
sudo pacman -S cmake sdl2_mixer sdl2_image sdl2 sdl2_ttf sdl2_gfx enet
```

## Raylib installation

```
git clone https://github.com/raysan5/raylib.git
cd raylib
mkdir build; cd build
cmake ..
make -j `nproc`
sudo make install
```
