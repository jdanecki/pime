#ifndef PLAYER_H
#define PLAYER_H

#include "alchemist/npc_talk.h"
#include <stdint.h>

/*enum class direction
{
    up,
    right,
    down,
    left,
};
*/
class Player;

enum Relations
{
    REL_unknown,
    REL_known,
};

struct PlayerRelation
{
    Player * who;
    enum Relations rel;
    struct PlayerRelation * next;
    PlayerRelation(Player *p, enum Relations r);
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
    //enum direction direction;
    int thirst;
    int hunger; // hungry, very hungry, full
    int nutrition;
    InvList * inventory;
    InventoryElement * hotbar[10];
    int craftbar[10];
    bool in_conversation;
    bool welcomed;
    Player * talking_to;
    PlayerRelation * relations;

    void pickup(InventoryElement * item);
    void drop(InventoryElement * item);
    InventoryElement * get_item_by_uid(size_t id);
    int get_id();
    Player(int id);
    int conversation(Player * who, Sentence * s, InventoryElement *el);
    void stop_conversation();

    void show(bool details = true);
    bool say(Sentence * s);
    Sentence *get_answer(Sentence * s);
    void ask(Sentence * s, InventoryElement * el);
    void ask(enum Npc_say s, InventoryElement * el);
    char * get_el_description(InventoryElement * el);
    void set_known(InventoryElement * el);
    bool conversation_started() { return in_conversation;}
    enum  Relations find_relation(Player *who);
    void set_relation(Player *who, enum Relations rel);
};


#endif
