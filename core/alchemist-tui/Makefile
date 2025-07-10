ALCHEMIST=../alchemist/elements.cpp ../alchemist/el_list.cpp ../alchemist/names.cpp ../alchemist/npc_talk.cpp ../alchemist/properties.cpp
TOOLS=../../server/cpp-src/tools/axe.cpp ../../server/cpp-src/tools/axe_blade.cpp ../../server/cpp-src/tools/axe_handle.cpp \
	  ../../server/cpp-src/tools/knife.cpp ../../server/cpp-src/tools/knife_blade.cpp ../../server/cpp-src/tools/knife_handle.cpp \
	  ../../server/cpp-src/tools/wall.cpp
CORE=../world.cpp ../tiles.cpp ../player.cpp ../npc.cpp
SERVER=../../server/cpp-src/elements_server.cpp

all:
	ccache g++ -Wall -Wno-switch -DCORE_TUI -g -o alchemist \
        $(ALCHEMIST) $(SERVER) $(TOOLS)  $(CORE) \
        main.cpp test_axe.cpp show_list.cpp game_time.cpp plants.cpp 

clean:
	rm alchemist

