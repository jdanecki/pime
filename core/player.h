#ifndef PLAYER_H
#define PLAYER_H

#include "alchemist/npc_talk.h"
#include <stdint.h>

enum class direction
{
    up,
    right,
    down,
    left,
};

class Player : public Being
{
    int * padding; // FIXME
    int id;

  protected:
    ElementsList * known_elements;

    // FIXME
    // add list know players, npc's
  public:
    int map_y;
    int map_x;
    int y;
    int x;
    char running;
    char sneaking;
    char going_right;
    enum direction direction;
    int thirst;
    int hunger; // hungry, very hungry, full
    int nutrition;
    InvList * inventory;
    InventoryElement * hotbar[10];
    int craftbar[10];
    bool conversation;
    bool welcomed;
    Player * talking_to;

    void pickup(InventoryElement * item);
    void drop(InventoryElement * item);
    InventoryElement * get_item_by_uid(size_t id);
    int get_id()
    {
        return id;
    }
    Player(int id);
    void start_conversation(Player * who)
    {
        conversation = true;
        printf("%s start talking to %s\n", get_name(), who->get_name());
        talking_to = who;
        who->talking_to = this;
    }
    void stop_conversation()
    {
        conversation = false;
        welcomed = false;
        printf("%s stopped talking to %s\n", talking_to->get_name(), get_name());
        talking_to = nullptr;
    }

    void show(bool details = true)
    {
        Being::show(true);
        if (talking_to)
        {
            printf("%s is talking to %s\n", get_name(), talking_to->get_name());
        }
    }
    bool say(Sentence * s);
    Sentence *get_answer(Sentence * s);
    void ask(Sentence * s, InventoryElement * el);
    void ask(enum Npc_say s, InventoryElement * el);
    char * get_el_description(InventoryElement * el);
    void set_known(InventoryElement * el);
};

#endif
