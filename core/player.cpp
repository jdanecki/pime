#include "tiles.h"
#include "player.h"

extern void print_status(int i, const char * format, ...);

const char * relations_names[]=
{
  "unknown", "known"
};

void Player::pickup(InventoryElement * item)
{
    inventory->add(item);

    ItemLocation location;
    location.type = LOCATION_PLAYER_INV;
    location.data.player.id = id;
    item->location = location;
}

void Player::drop(InventoryElement * item)
{
    inventory->remove(item);
}

InventoryElement * Player::get_item_by_uid(size_t id)
{
    ListElement * cur = inventory->head;
    while (cur)
    {
        if (cur->el->uid == id)
            return cur->el;
        cur = cur->next;
    }
    return NULL;
}

Player::Player(int id) : id(id)
{
    c_id = Class_Player;
    hunger = 500;
    thirst = 250;
    map_x = WORLD_CENTER;
    map_y = WORLD_CENTER;
    inventory = new InvList("inventory");
    relations = nullptr;
    //direction = direction::right;

    x = 0;
    y = 0;

    for (int i = 0; i < 10; i++)
    {
        hotbar[i] = NULL;
        craftbar[i] = 0;
    }
    alive = true;
    max_age = new Property("max age", 1 + rand() % 180000);
    age = new Property("age", rand() % max_age->value);
    can_talk = true;
    conversation = false;
    talking_to = nullptr;
    welcomed = false;
    delete name;
    name = new char[16];
    sprintf((char *)name, "%s%d", "Player", id);

    known_elements = new ElementsList("known elements");
    known_elements->add(new ElementsTable(BASE_ELEMENTS, Class_BaseElement));
    known_elements->add(new ElementsTable(BASE_ANIMALS, Class_BaseAnimal));
    known_elements->add(new ElementsTable(BASE_PLANTS, Class_BasePlant));
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
            talking_to->welcomed = true;
        // pass through
        default:        
            talking_to->get_answer(s);
            break;

    }
    return false;
}

Sentence * Player::get_answer(Sentence *s)
{
    Npc_say sid=NPC_Say_Hello;
    Sentence * a=dynamic_cast<Sentence *>(sentences->find(&sid));

    const char * n;
    if (talking_to->find_relation(this) == REL_known)
        n=get_name();
    else n=get_class_name();

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
    if (talking_to)
        talking_to->ask(s->id, el);
}

void Player::ask(enum Npc_say s, InventoryElement * el)
{
    Npc_say sid=NPC_Say_Nothing;
    Sentence * a;
    const char * n;
    Relations player_rel=talking_to->find_relation(this);
     if (player_rel == REL_known) n=get_name();
        else n=get_class_name();

    if (s == NPC_Ask_do_you_know_item)
    {
            char * des = get_el_description(el);
            if (des) sid=NPC_Answer_I_know_it;
            else
                sid=NPC_Answer_I_dont_know_it;

            a=dynamic_cast<Sentence *>(answers->find(&sid));
            if (des)
            {
                print_status(1, "%s says: %s. It's %s", n, a->text, des);
                talking_to->set_known(el);                
            }
            else
            {
                print_status(1, "%s says: %s", n, a);
            }
    }
    else {
        switch (s)
        {
            case NPC_Ask_do_we_know_each_other:
                if (player_rel == REL_known)
                sid = NPC_Answer_I_know_you;
                    else
                sid = NPC_Answer_I_dont_know_you;
                break;
            case NPC_Ask_how_are_you: sid = NPC_Answer_Im_fine; break;
            case NPC_Ask_where_am_I: sid=NPC_Answer_You_are_in_pime; break;
            case NPC_Ask_who_are_you:
                print_status(1, "%s says: I'm %s", n, get_name());
                talking_to->set_relation(this, REL_known);
                break;
        }
        a=dynamic_cast<Sentence *>(answers->find(&sid));
        if (a)
            print_status(1, "%s says: %s", n, a->text);
    }
}

char * Player::get_el_description(InventoryElement * el)
{
    if (el->crafted)
        return el->get_description();

    Class_id b = el->get_base_cid();
    ElementsTable * known_list = dynamic_cast<ElementsTable *>(known_elements->find(&b));
    bool known = known_list->is_known(el->get_id());

    if (known)
        return el->get_description();
    else
        return nullptr;
}

void Player::set_known(InventoryElement * el)
{
    Class_id b = el->get_base_cid();
    ElementsTable * known_list = dynamic_cast<ElementsTable *>(known_elements->find(&b));
    known_list->set_known((el->get_id()));
}

Relations Player::find_relation(Player *who)
{
    if (relations)
    {
        PlayerRelation *p=relations;
        while(p)
        {
            if (p->who == who) return p->rel;
            p=p->next;
        }
    }
    return REL_unknown;
}

void Player::set_relation(Player *who, enum Relations rel)
{
    if (relations)
    {
        PlayerRelation *p=relations;
        while(p)
        {
            if (p->who == who) {
                p->rel = rel;
                return;
            }
            p=p->next;
        }
        PlayerRelation * new_rel=new PlayerRelation(who, rel);
        new_rel->next=relations;
        relations=new_rel;
    }
    else
    {
        relations=new PlayerRelation(who, rel);
    }

}
