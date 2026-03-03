#include <string.h>

#include "alchemist/object.h"
#include "clan.h"
#include "player.h"
#include "world.h"

extern void print_status(int i, const char * format, ...);

const char * relations_names[] = {"unknown", "known"};

void Player::pickup(InventoryElement * item)
{
    inventory.add(item);
    CONSOLE_LOG("player %s pickup inv=%s (%lx)\n", get_name(), item->get_class_name(), item->get_uid());
    ItemLocation location;
    location.tag = ItemLocation::Tag::Player;
    location.player.id = uid;
    item->location = location;
}

void Player::drop(InventoryElement * item)
{
    inventory.remove(item);
}

InventoryElement * Player::get_item_by_uid(size_t id)
{
    for (InventoryElement * el : inventory)
    {
        if (el->uid == id)
            return el;
    }
    return NULL;
}

size_t Player::get_id()
{
    return uid;
}

Player::Player(size_t uid, SerializableCString && name, ItemLocation location, int thirst, int hunger, int nutrition)
    : InventoryElement(Class_Player, uid, location), name(name), thirst(thirst), hunger(hunger), nutrition(nutrition),
	  inventory("inventory"), known_elements("known elements"),
      clan(get_clan_by_id(Clan_Elf)), talking_to(nullptr)
{
    CONSOLE_LOG("new player: uid = %ld name=%s\n", uid, get_name());
    // FIXME
    // relations = nullptr;

    in_conversation = false;
    welcomed = false;

    checked_element = 0;
    memcpy(player_skills, clan.get()->skills, sizeof(player_skills));
    running = 0;
    sneaking = 0;
    dimensions.length.value = 1;
    dimensions.width.value = 1;
    dimensions.height.value = 1;
    dimensions.volume.value = 1;
}

bool Player::conversation(Sentence * s, InventoryElement * el)
{
    if (s->id < NPC_Say_Nothing)
    {
        ask(s, el);
        return true;
    }
    else
        return say(s);
}

void Player::start_conversation(Player *who)
{
    in_conversation = true;
    talking_to.set(who);
    CONSOLE_LOG("%s (uid=%lx) starts talking to %s (%p uid=%lx)\n", get_name(), get_uid(), who->get_name(), who, who->get_uid());
 /*   talking_to.show();
    who->talking_to.show();
*/
}
void Player::stop_conversation()
{
    in_conversation = false;
    welcomed = false;
    talking_to.set(nullptr);
}

void Player::show(bool details)
{
    CONSOLE_LOG("%s %s (%p) clan=%s id=%ld @ [%d,%d]:[%f,%f] <%c %c>\n", class_name[c_id], get_name(), this, clan_names[clan.get()->id], get_id(), location.chunk.map_x, location.chunk.map_y, location.chunk.x,
        location.chunk.y, running ? 'R' : ' ', sneaking ? 'S' : ' ');
    if (details)
    {
        // FIXME
        // player_skills.show(true);
        if (in_conversation) {
            CONSOLE_LOG("in conversation, welcomed=%d\n", welcomed);
            Player *t= talking_to.get();
            if (t)
            {
                CONSOLE_LOG("%s is talking to %s id=%ld\n", get_name(), t->get_name(), t->get_id());
            }
        }
        CONSOLE_LOG("inventory: %d elements\n", inventory.nr_elements);
    }
}

bool Player::say(Sentence * s)
{
    if (!s)
        return false;
    Player * t=talking_to.get();

    switch (s->id)
    {
        case NPC_Say_Bye:
            return false;

        case NPC_Say_Hello:
            t->welcomed = true;
        // pass through
        default:
            t->get_answer(s);
            break;
    }
    return true;
}

Sentence * Player::get_answer(Sentence * s)
{
    CONSOLE_LOG("player: %s can't answer\n", get_name());
    return nullptr;
}

void Player::ask(Sentence * s, InventoryElement * el)
{
    Player * t=talking_to.get();
    if (t)
        t->ask(s->id, el);
}

void Player::ask(enum Npc_say s, InventoryElement * el)
{
    CONSOLE_LOG("Can't ask player %s\n", get_name());
}

char * Player::get_el_description(InventoryElement * el)
{
    // FIXME
    // if (check_known(el))
    return el->get_description();
    // else
    return nullptr;
}

bool Player::check_known(InventoryElement * el)
{
    ElId i;
    i.c_id = el->get_base_cid();
    i.id = el->get_id();

    KnownElement * k = static_cast<KnownElement *>(known_elements.find(&i));
    if (!k)
        return false;
    return k->is_known();
}

bool Player::set_known(Class_id cid, int el_id)
{
    ElId i;
    i.c_id = cid;
    i.id = el_id;

    KnownElement * k = static_cast<KnownElement *>(known_elements.find(&i));
    if (!k)
    {
        KnownElement * n = new KnownElement(i.c_id, i.id);
        known_elements.add(n);
        n->set_known();
        return true;
    }
    else
        return false;
}

bool Player::set_checked(size_t el)
{
    if (el != checked_element)
    {
        checked_element = el;
        return true;
    }
    else
        return false;
}

Relations Player::find_relation(Player * who)
{
    // FIXME
    // if (relations)
    // {
    //     PlayerRelation * p = relations;
    //     while (p)
    //     {
    //         if (p->who == who)
    //             return p->rel;
    //         p = p->next;
    //     }
    // }
    return REL_unknown;
}

void Player::set_relation(Player * who, enum Relations rel)
{
    // FIXME
    // if (relations)
    // {
    //     PlayerRelation * p = relations;
    //     while (p)
    //     {
    //         if (p->who == who)
    //         {
    //             p->rel = rel;
    //             return;
    //         }
    //         p = p->next;
    //     }
    //     PlayerRelation * new_rel = new PlayerRelation(who, rel);
    //     new_rel->next = relations;
    //     relations = new_rel;
    // }
    // else
    // {
    //     relations = new PlayerRelation(who, rel);
    // }
}

PlayerRelation::PlayerRelation(Player * p, Relations r)
{
    who = p;
    rel = r;
    next = nullptr;
}
ElementsList * Player::get_known_elements()
{
    return &known_elements;
}

bool Player::check_conversation()
{
    return in_conversation;
}
const char * Player::get_name()
{
    return name.str;
}
