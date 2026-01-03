ALCHEMIST=../alchemist/elements.cpp ../alchemist/el_list.cpp ../alchemist/names.cpp ../alchemist/npc_talk.cpp ../alchemist/properties.cpp \
                  ../alchemist/skills.cpp ../alchemist/serialization.cpp
TOOLS=../../server/cpp-src/tools/axe.cpp ../../server/cpp-src/tools/axe_blade.cpp ../../server/cpp-src/tools/axe_handle.cpp \
	  ../../server/cpp-src/tools/knife.cpp ../../server/cpp-src/tools/knife_blade.cpp ../../server/cpp-src/tools/knife_handle.cpp \
	  ../../server/cpp-src/tools/pickaxe.cpp ../../server/cpp-src/tools/pickaxe_blade.cpp ../../server/cpp-src/tools/pickaxe_handle.cpp \
	  ../../server/cpp-src/tools/wall.cpp ../../server/cpp-src/tools/hut.cpp ../../server/cpp-src/tools/meat.cpp ../../server/cpp-src/tools/log.cpp \
	  ../../server/cpp-src/tools/tinder.cpp ../../server/cpp-src/tools/fruit.cpp ../../server/cpp-src/tools/fruit_salad.cpp ../../server/cpp-src/tools/fire.cpp \
	  ../../server/cpp-src/tools/roasted_meat.cpp ../../server/cpp-src/tools/stick.cpp

CORE=../world.cpp ../tiles.cpp ../player.cpp ../npc.cpp ../clan.cpp
SERVER=../../server/cpp-src/elements_server.cpp ../../server/cpp-src/networking.cpp ../../server/cpp-src/craft_ing.cpp ../../server/cpp-src/craft_prod.cpp
TEST=test_axe.cpp test_knife.cpp

all:
	ccache g++ -Wall -Wno-switch -DCORE_TUI -g -o alchemist \
        $(ALCHEMIST) $(SERVER) $(TOOLS)  $(CORE) \
        main.cpp $(TEST) show_list.cpp game_time.cpp plants.cpp 

clean:
	rm -f alchemist

