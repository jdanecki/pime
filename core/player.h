#ifndef PLAYER_H
#define PLAYER_H

#include "alchemist/elements.h"
#include "alchemist/npc_talk.h"

#include "clan.h"
#include "alchemist/skills.h"

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

enum Emotions
{
    EMO_UNKNOWN,
    EMO_ANGER,
    EMO_FEAR,
    EMO_DISGUST,
    EMO_JOY,
    EMO_SADNESS
};

struct PlayerRelation
{
    Player * who;
    enum Relations rel;
    struct PlayerRelation * next;
    PlayerRelation(Player * p, enum Relations r);
};

class Player : public InventoryElement
{
    int * padding; // FIXME
#ifdef USE_ENET
  public:
#else
  protected:
#endif
    SerializableCString name;

    // FIXME
    // add list know players, npc's
  public:
    char running;
    char sneaking;
    char going_right;
    // enum direction direction;
    int thirst;
    int hunger; // hungry, very hungry, full
    int nutrition;
    InvList * inventory;
    InventoryElement * hotbar[10];
    ElementsList * known_elements;
    size_t checked_element;

    Clan * clan;
    Skills * player_skills;

    int craftbar[10];
    bool in_conversation;
    bool welcomed;
    Player * talking_to;
    PlayerRelation * relations;

    void pickup(InventoryElement * item);
    void drop(InventoryElement * item);
    InventoryElement * get_item_by_uid(size_t id);
    int get_id();
    // Player(int id);
    Player(size_t id, SerializableCString&& name, ItemLocation location, int thirst, int hunger, int nutrition);
    int conversation(Player * who, Sentence * s, InventoryElement * el);
    void stop_conversation();

    void show(bool details = true);
    bool say(Sentence * s);
    Sentence * get_answer(Sentence * s);
    void ask(Sentence * s, InventoryElement * el);
    void ask(enum Npc_say s, InventoryElement * el);
    char * get_el_description(InventoryElement * el);

    virtual bool check_known(InventoryElement * el);

    bool set_known(Class_id cid, int el_id);

    bool conversation_started()
    {
        return in_conversation;
    }
    enum Relations find_relation(Player * who);
    void set_relation(Player * who, enum Relations rel);

    const char * get_name()
    {
        return name.str;
    }
    bool set_checked(size_t el);
};

#endif
