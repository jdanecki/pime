CXX := g++
CXXFLAGS := -g -DUSE_ENET
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf
OBJDIR := build

SDL_FILES := ../window.cpp ../text.cpp

SRC := test_textures.cpp $(SDL_FILES)
OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(SRC)))

all: $(OBJDIR) $(OBJDIR)/test_textures

$(OBJDIR):
	mkdir build
	ln -s .. build/textures

$(OBJDIR)/test_textures: $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: ../%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

