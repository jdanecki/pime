SDL_PKG=sdl2 SDL2_image SDL2_ttf SDL2_gfx
SDL_CXXFLAGS := `pkg-config --cflags $(SDL_PKG)`
SDL_LIBS=`pkg-config --libs $(SDL_PKG)`

RAYLIB_CXXFLAGS := `pkg-config --cflags raylib`
RAYLIB_LIBS=`pkg-config --libs raylib`


CXXFLAGS=-g

CLIENT_FILES=../../client-common/text.cpp ../../client-common/texture.cpp ../../client-common/alchemist2d.cpp 
DIALOG_FILES=../dialog.cpp ../d_craft.cpp ../d_action.cpp

SDL_FILES=../../SDL/window.cpp ../../SDL/text.cpp ../../SDL/backend.cpp handle_events_sdl.cpp
RAYLIB_FILES=../../raylib/window.cpp ../../raylib/text.cpp ../../raylib/backend.cpp handle_events_raylib.cpp

SOURCES=test_dialog.cpp $(DIALOG_FILES) $(CLIENT_FILES)

all: core test_dialog_sdl test_dialog_raylib

core:
	rm -rf build; mkdir build
	cd build; cmake ../../../core ; make -j 12

test_dialog_sdl: $(SOURCES) $(SDL_FILES)
	g++ -I../../SDL $(SOURCES) $(SDL_FILES) $(CXXFLAGS) -Lbuild -lpime -o $@ $(SDL_CXXFLAGS) $(SDL_LIBS) 

test_dialog_raylib: $(SOURCES) $(RAYLIB_FILES)
	g++ -I../../raylib $(SOURCES) $(RAYLIB_FILES) $(CXXFLAGS) -Lbuild -lpime -o $@ $(RAYLIB_CXXFLAGS) $(RAYLIB_LIBS) 

clean:
	rm -f test_dialog_sdl test_dialog_raylib

