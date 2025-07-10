#include "player.h"
#include "tiles.h"

extern void print_status(int i, const char * format, ...);

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
    direction = direction::right;

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
    if (!talking_to)
    {
        print_status(1, "conversation not started yet!");
        return false;
    }
    switch (s->id)
    {
        case NPC_Say_Hello:
        case NPC_Ask_how_are_you:
            sentences->enable(NPC_Say_Bye);
            talking_to->welcomed = true;
        // pass through
        default:
        {
            Sentence * answer = talking_to->get_answer(s);

            switch (answer->id)
            {
                default:
                    print_status(0, "%s answers: %s", talking_to->get_name(), answer->text);
                    break;
                case NPC_Say_Im:
                    print_status(0, "%s answers: %s %s", talking_to->get_name(), answer->text, talking_to->get_name());
                    break;
            }
           // s->disable();
            switch (answer->id)
            {
                case NPC_Say_Nothing:
                    break;
                case NPC_Ask_do_we_know_each_other:
                    print_status(1, "%s: Don't you remember me? I'm..., we've met some time ago.", get_name());
                    break;
                case NPC_Say_I_dont_know_you:
                    print_status(1, "%s: Let me introduce myself...", get_name());
                    break;
                case NPC_Say_Im_not_fine:
                    print_status(1, "%s: I'm sorry...", get_name());
                    break;
                case NPC_Ask_do_you_really_care:
                    print_status(1, "%s: Not really... Hmm, I was kidding.", get_name());
                    break;
                case NPC_Ask_why_do_you_ask:
                    print_status(1, "%s: I'm interrested.", get_name());
                    break;
                case NPC_Ask_whos_asking:
                    print_status(1, "%s: It's me.", get_name());
                    break;
                case NPC_Ask_have_you_lost:
                    print_status(1, "%s: Yes", get_name());
                    break;
                case NPC_Ask_dont_you_remember:
                    print_status(1, "%s: No, I don't remember", get_name());
                    break;
            }

            if (s->id == NPC_Say_Bye)
            {

                stop_conversation();
                return true;
            }
            break;
        }
    }
    return false;
}

Sentence * Player::get_answer(Sentence *s)
{
    Npc_say sid=NPC_Say_Nothing;
    Sentence * a=dynamic_cast<Sentence *>(sentences->find(&sid));
    return a;
}

void Player::ask(Sentence * s, InventoryElement * el)
{
    if (talking_to)
        talking_to->ask(s->id, el);
}

void Player::ask(enum Npc_say s, InventoryElement * el)
{
    switch (s)
    {
        case NPC_Ask_do_you_know_item:
            char * des = get_el_description(el);
            if (des)
            {
                print_status(1, "%s says: I know it. It's %s", get_name(), des);
                talking_to->set_known(el);
            }
            else
                print_status(1, "%s says: I don't know it", get_name());
            break;
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
