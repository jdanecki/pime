#include "npc.h"

// defined in SDL/main.h
// #define PLAYER_NUM 16
#include "alchemist/random_functions.h"
extern void print_status(int i, const char * format, ...);

int npcs_count;

Npc::Npc(size_t uid, ItemLocation location) : Player(uid, SerializableCString(create_name(random_range(3, 5))), location, 100, 100, 100)
{
    c_id = Class_Npc;
}
bool Npc::check_known(InventoryElement * el)
{
    return true;
}

void Npc::ask(Npc_say s, InventoryElement *el)
{
    Npc_say sid = NPC_Say_Nothing;
    Sentence * a;
    const char * n;
    Player * t= talking_to.get();
    Relations player_rel = t->find_relation(this);
    if (player_rel == REL_known)
        n = get_name();
    else
        n = get_class_name();

    if (s == NPC_Ask_do_you_know_inv_item)
    {
        char * des = get_el_description(el);
        if (des)
            sid = NPC_Answer_I_know_it;
        else
            sid = NPC_Answer_I_dont_know_it;

        a = static_cast<Sentence *>(answers->find(&sid));
        if (des)
        {
            print_status(1, "%s says: %s. It's %s", n, a->text, des);
          //  t->set_known(el->get_base_cid(), el->get_id());
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
                t->set_relation(this, REL_known);
                break;
            case NPC_Ask_what_do_you_have:
            	print_status(1, "%s says: I've %d items", n, inventory.nr_elements);
            	break;

        }
        a = static_cast<Sentence *>(answers->find(&sid));
        if (a)
            print_status(1, "%s says: %s", n, a->text);
    }

}

bool Npc::say(Sentence *s)
{
    CONSOLE_LOG("Npc: %s can't start talking\n", get_name());
    return true;
}

Sentence *Npc::get_answer(Sentence *s)
{
    Npc_say sid = NPC_Say_Hello;
    Sentence * a = static_cast<Sentence *>(sentences->find(&sid));

    const char * n;
    if (talking_to.get()->find_relation(this) == REL_known)
        n = get_name();
    else
        n = get_class_name();

    switch (s->id)
    {
        default:
            print_status(1, "%s answers: %s", n, a->text);
            break;
    }
    return a;

}

void Npc::show(bool details)
{
    Player::show(details);
}
