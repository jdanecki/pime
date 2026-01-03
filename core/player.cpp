#include "clan.h"
#include "tiles.h"
#include "player.h"
#include <string.h>

extern void print_status(int i, const char * format, ...);

const char * relations_names[] = {"unknown", "known"};

void Player::pickup(InventoryElement * item)
{
    inventory.add(item);
    CONSOLE_LOG("player %p pickup inv=%lx\n", this, item->get_uid());
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

// Player::Player(int id) : InventoryElement(Class_Player), id(id), name(new char[16])
// {
// hunger = 500;
// thirst = 250;
// map_x = WORLD_CENTER;
// map_y = WORLD_CENTER;
// inventory = new InvList("inventory");
// relations = nullptr;
// // direction = direction::right;

// x = 8;
// y = 8;

// for (int i = 0; i < 10; i++)
// {
//     hotbar[i] = NULL;
//     craftbar[i] = 0;
// }
// in_conversation = false;
// talking_to = nullptr;
// welcomed = false;
// sprintf((char*)name.str, "%s%d", "Player", id);

// known_elements = new ElementsList("known elements");
// }

Player::Player(size_t uid, SerializableCString && name, ItemLocation location, int thirst, int hunger, int nutrition)
    : InventoryElement(Class_Player, uid, location), name(name), thirst(thirst), hunger(hunger), nutrition(nutrition), inventory("inventory"), known_elements("known elements"),
      clan(get_clan_by_id(Clan_Human)), talking_to(nullptr)
{
    CONSOLE_LOG("new player: uid = %ld name=%s\n", uid, get_name());
    // FIXME
    // relations = nullptr;

    in_conversation = false;
    welcomed = false;

    checked_element = 0;
    memcpy(player_skills, clan.get()->skills, sizeof(player_skills));
    show(true);
}

int Player::conversation(Player * who, Sentence * s, InventoryElement * el)
{
    if (!in_conversation)
    {
        in_conversation = true;
        CONSOLE_LOG("%s start talking to %s\n", get_name(), who->get_name());
        talking_to = who;
        who->talking_to = this;
    }
    if (s->id < NPC_Say_Nothing)
    {
        ask(s, el);
        return 0;
    }
    else
    {

        return say(s) ? 1 : 0;
    }
}

void Player::stop_conversation()
{
    in_conversation = false;
    welcomed = false;
    if (talking_to.get())
    {
        CONSOLE_LOG("%s stopped talking to %s\n", talking_to.get()->get_name(), get_name());
        Player * p = talking_to.get();
        talking_to = nullptr;
        p->stop_conversation();
    }
}

void Player::show(bool details)
{
    CONSOLE_LOG(
        "%s %s clan=%s id=%ld @ [%d,%d]:[%d,%d]\n", class_name[c_id], get_name(), clan_names[clan.get()->id], get_id(), location.chunk.map_x, location.chunk.map_y, location.chunk.x, location.chunk.y);
    if (details)
    {
        // FIXME
        // player_skills.show(true);
        if (get_talking_to())
        {
            CONSOLE_LOG("%s is talking to %s\n", get_name(), get_talking_to()->get_name());
        }
    }
}

bool Player::say(Sentence * s)
{
    if (!s)
        return false;
    switch (s->id)
    {
        case NPC_Say_Bye:
        case NPC_Say_See_you_later:
        case NPC_Say_See_you_next_time:
            stop_conversation();
            return true;

        case NPC_Say_Hello:
            get_talking_to()->welcomed = true;
        // pass through
        default:
            get_talking_to()->get_answer(s);
            break;
    }
    return false;
}

Sentence * Player::get_answer(Sentence * s)
{
    Npc_say sid = NPC_Say_Hello;
    Sentence * a = dynamic_cast<Sentence *>(sentences->find(&sid));

    const char * n;
    if (get_talking_to()->find_relation(this) == REL_known)
        n = get_name();
    else
        n = get_class_name();

    switch (s->id)
    {
        default:
            print_status(0, "%s answers: %s", n, a->text);
            break;
    }
    return a;
}

void Player::ask(Sentence * s, InventoryElement * el)
{
    if (get_talking_to())
        get_talking_to()->ask(s->id, el);
}

void Player::ask(enum Npc_say s, InventoryElement * el)
{
    Npc_say sid = NPC_Say_Nothing;
    Sentence * a;
    const char * n;
    Relations player_rel = get_talking_to()->find_relation(this);
    if (player_rel == REL_known)
        n = get_name();
    else
        n = get_class_name();

    if (s == NPC_Ask_do_you_know_inv_item || s == NPC_Ask_do_you_know_item)
    {
        char * des = get_el_description(el);
        if (des)
            sid = NPC_Answer_I_know_it;
        else
            sid = NPC_Answer_I_dont_know_it;

        a = dynamic_cast<Sentence *>(answers->find(&sid));
        if (des)
        {
            print_status(1, "%s says: %s. It's %s", n, a->text, des);
            get_talking_to()->set_known(el->get_base_cid(), el->get_id());
        }
        else
        {
            print_status(1, "%s says: %s", n, a->text);
        }
    }
    else
    {
        switch (s)
        {
            case NPC_Ask_do_we_know_each_other:
                if (player_rel == REL_known)
                    sid = NPC_Answer_I_know_you;
                else
                    sid = NPC_Answer_I_dont_know_you;
                break;
            case NPC_Ask_how_are_you:
                sid = NPC_Answer_Im_fine;
                break;
            case NPC_Ask_where_am_I:
                sid = NPC_Answer_You_are_in_pime;
                break;
            case NPC_Ask_who_are_you:
                print_status(1, "%s says: I'm %s", n, get_name());
                get_talking_to()->set_relation(this, REL_known);
                break;
        }
        a = dynamic_cast<Sentence *>(answers->find(&sid));
        if (a)
            print_status(1, "%s says: %s", n, a->text);
    }
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

    KnownElement * k = dynamic_cast<KnownElement *>(known_elements.find(&i));
    if (!k)
        return false;
    return k->is_known();
}

bool Player::set_known(Class_id cid, int el_id)
{
    ElId i;
    i.c_id = cid;
    i.id = el_id;

    KnownElement * k = dynamic_cast<KnownElement *>(known_elements.find(&i));
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
