CXX := g++
CXXFLAGS := -g
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -L../build/core -lpime \
           -Wl,-rpath,'$$ORIGIN/../build/core'

SDL_FILES := ../window.cpp ../text.cpp ../texture.cpp ../implementations/alchemistSDL.cpp \
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

