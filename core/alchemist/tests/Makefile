ALCHEMIST=../el_list.cpp ../elements.cpp ../serialization.cpp ../properties.cpp \
		  ../object.cpp ../skills.cpp ../names.cpp 
CORE=../../clan.cpp ../../world.cpp ../../tiles.cpp

CFLAGS=-Wall -Wno-switch -Wno-reorder -g  -I..

all: build
	g++ $(CFLAGS) $(ALCHEMIST) $(CORE) test_lists.cpp -o build/test_lists

build:
	mkdir build

clean:	
	rm -rf build

