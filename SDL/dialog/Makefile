CXX := g++
PKG=sdl2 SDL2_image SDL2_ttf SDL2_gfx
CXXFLAGS := `pkg-config --cflags $(PKG)` -g
LIBS=`pkg-config --libs $(PKG)`
LDFLAGS := $(LIBS) -L../build/core -lpime \
           -Wl,-rpath,'$$ORIGIN/../build/core'

SDL_FILES := ../window.cpp ../text.cpp ../texture.cpp ../alchemistSDL.cpp \
             ../palette.cpp

SRC := test_dialog.cpp dialog.cpp d_craft.cpp d_action.cpp $(SDL_FILES)
OBJ := $(SRC:.cpp=.o)

all: test_dialog

test_dialog: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) test_dialog

