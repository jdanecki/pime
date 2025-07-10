#include "../alchemist/el_list.h"
#include "../alchemist/npc_talk.h"
#include "../npc.h"
#include "game_time.h"
#include "plants.h"
#include "show_list.h"

#include "test_axe.h"
#include "../../server/cpp-src/tools/wall.h"

#include <cstdio>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

InvList * elements;
InvList * animals;
InvList * npcs;

Player * player;

Npc * current_npc;

void (*callback_daily)();

void daily_call()
{
    printf("daily call\n");
    plants->tick();
    animals->tick();
}

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}
void notify_destroy(size_t id, ItemLocation location)
{
}

void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
{
}

void help()
{
    printf("%sESC/e/?- clear screen/Exit/Help\n", colorWhite);

    printf("c - Craft\n");
    printf("h - Harvest plant\n");
    printf("o - sOw plant\n");
    printf("u - hUnt animal\n");
    printf("1 - use item from inventory\n");

    printf("s - Show\n");
    printf("& - change clock\n");

    printf("f - Find\n");
    printf("p - Pickup up element\n");
    printf("d - Drop element\n");

    printf("# - conversation\n");
    printf("@ - ask questions\n");

    printf("t - Test\n");
}

void show_description()
{
    printf("Which item do you want to describe?\n");
    InventoryElement * el = select_element(player->inventory);
    if (!el)
        return;
    char * des = player->get_el_description(el);
    if (des)
        printf("%s\n", des);
    else
        printf("It looks like %s, it has %s form, but I don't know what it's exactly\n",
            el->get_class_name(), el->get_form_name());
}

void show()
{
    printf("%sb/B - base elements (details off/on)\n", colorCyan);
    printf("1/! - base animals (details off/on)\n");
    printf("2/@ - base plants (details off/on)\n");

    printf("e/E - elements (details off/on)\n");
    printf("i/I - inventory (details off/on)\n");
    printf("p/P - plants (details off/on)\n");
    printf("a/A - animals (details off/on)\n");
    printf("n/N - npcs/player (details off/on)\n");
    printf("d - item description\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('s');
    switch (c)
    {
        case 'b':
        case 'B':
            show_base_table(Class_BaseElement, c == 'B');
            break;

        case '1':
        case '!':
            show_base_table(Class_BaseAnimal, c == '!');
            break;

        case '2':
        case '@':
            show_base_table(Class_BasePlant, c == '@');
            break;

        case 'i':
        case 'I':
            player->inventory->show(c == 'I');
            break;

        case 'e':
        case 'E':
            elements->show(c == 'E');
            break;

        case 'a':
        case 'A':
            animals->show(c == 'A');
            break;

        case 'p':
        case 'P':
            plants->show(c == 'P');
            break;

        case 'n':
        case 'N':
            npcs->show(c == 'N');
            player->show(c == 'N');
            break;

        case 'd':
            show_description();
            break;
    }
}
void add_new_element()
{
    Element * el = new Element;
    elements->add(el);
    printf("new Element %s found\n", el->get_name());
}

void add_new_animal()
{
    Animal * el = new Animal;
    animals->add(el);
    printf("new Animal %s found\n", el->get_name());
}

void add_new_plant()
{
    Plant * p = new Plant;
    plants->add(p);
    printf("new Plant %s found\n", p->get_name());
}

void add_new_npc()
{
    Npc * el = new Npc;
    npcs->add(el);
    printf("new NPC %s found\n", el->get_name());
}

void find_new()
{
    printf("%se - Element\n", colorCyan);
    printf("p - Plant\n");
    printf("a - Animal\n");
    printf("n - Npc\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('f');
    switch (c)
    {
        case 'e':
            add_new_element();
            break;
        case 'p':
            add_new_plant();
            break;
        case 'a':
            add_new_animal();
            break;
        case 'n':
            add_new_npc();
            break;
    }
}

void change_clock()
{
    printf("%sh - add 1 hour\n", colorCyan);
    printf("d - add 1 day\n");
    printf("m - add 30 days\n");
    printf("q - add 90 days\n");
    printf("y - add 1 year\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('@');
    switch (c)
    {
        case 'h':
            game_time->update_time(3600);
            break;
        case 'd':
            game_time->update_time(24 * 3600);
            break;
        case 'm':
            game_time->update_time(30 * 24 * 3600);
            break;
        case 'q':
            game_time->update_time(90 * 24 * 3600);
            break;
        case 'y':
            game_time->update_time(360 * 24 * 3600);
            break;
    }
}

void clone()
{
    InventoryElement * el = select_element(player->inventory);
    if (!el)
        return;
    el->show();
    Class_id cid = el->get_base_cid();
    InventoryElement * new_el;
    switch (cid)
    {
        case Class_BaseElement:
            new_el = new Element(el->get_id());
            break;
        case Class_BaseAnimal:
            new_el = new Animal(el->get_id());
            break;
        case Class_BasePlant:
            new_el = new Plant(el->get_id());
            break;
    }
    if (!new_el)
        return;
    player->inventory->add(new_el);
    new_el->show();
}

void test()
{
    printf("%sa - test axe\n", colorCyan);
    printf("c - clone item\n");

    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('t');
    switch (c)
    {
        case 'a':
        {
            Axe * axe = test_axe();
            if (axe)
                player->inventory->add(axe);
        }
        break;
        case 'c':
            clone();
            break;
    }
}

void craft()
{
    printf("%sa - craft axe\n", colorCyan);
    printf("b - craft axe blade\n");
    printf("h - craft axe handle\n");
    printf("w - craft wall\n");

    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('c');
    switch (c)
    {
        case 'a':
        {
            InventoryElement *el2[2];
            el2[0]= nullptr;
            el2[1] = nullptr;
            if (!select_inventory2(2, el2))
                return;
            Axe * axe = new Axe(el2[0], el2[1]);
            if (!axe->craft())
                return;
            player->inventory->add(axe);
            player->inventory->remove(el2[0]);
            player->inventory->remove(el2[1]);
            printf("axe added to inventory\n");
        }
        break;
        case 'b':
        case 'h':
        case 'w':
        {
            InventoryElement * el = select_element(player->inventory);
            if (!el)
                return;
            InventoryElement * target;
            switch (c)
            {
                case 'b':
                    target = new AxeBlade(el);
                    break;
                case 'h':
                    target = new AxeHandle(el);
                    break;
                case 'w':
                    target = new Wall(el);
                    break;
            }
            if (!target->craft())
            {
                delete target;
                return;
            }
            player->inventory->add(target);
            player->inventory->remove(el);
            printf("%s added to inventory\n", target->get_name());
        }        
        break;
    }
}

void pickup()
{
    printf("Which item do you want to pick up?\n");
    InventoryElement * el = select_element(elements);
    if (!el)
        return;
    player->inventory->add(el);
    elements->remove(el);
    printf("%s added to inventory\n", el->get_name());
}

void drop()
{
    printf("Which item do you want to drop?\n");
    InventoryElement * el = select_element(player->inventory);
    if (!el)
        return;
    printf("%s dropped from inventory\n", el->get_name());
    elements->add(el);
    player->inventory->remove(el);
}

void use()
{
    printf("Which item do you want to use?\n");
    InventoryElement * el = select_element(player->inventory);
    if (!el)
        return;
    Product * product = dynamic_cast<Product *>(el);
    if (!product)
    {
        printf("You can't use %s, make product from it\n", el->get_name());
        return;
    }
    printf("On what do you want to use %s\n", el->get_name());

    printf("%se/E - elements (details off/on)\n", colorCyan);
    printf("p/P - plants (details off/on)\n");
    printf("a/A - animals (details off/on)\n");
    printf("n/N - npcs/player (details off/on)\n");
    printf("%s%s", colorNormal, colorGreenBold);

    InventoryElement * obj;
    char c = wait_key('s');
    switch (c)
    {
        case 'e':
        case 'E':
            obj = select_element(elements);
            break;

        case 'a':
        case 'A':
            obj = select_element(animals);
            break;

        case 'p':
        case 'P':
            obj = select_element(plants);
            break;

        case 'n':
        case 'N':
            obj = select_element(npcs);
            break;
    }
    if (!obj)
        return;

    product->use(obj);

  //  player->inventory->remove(el);
}

void ask()
{
    if (!current_npc)
    {
        printf("Who do you want to talk to?\n");
        current_npc = dynamic_cast<Npc *>(select_element(npcs));
        questions->enable_all();
        player->start_conversation(current_npc);
    }
    if (current_npc)
    {
        if (!player->inventory->nr_elements)
            questions->disable(NPC_Ask_do_you_know_item);
        else
        {
            questions->enable(NPC_Ask_do_you_know_item);
        }
        Sentence * s = dynamic_cast<Sentence *>(select_list_element(questions));

        printf("%s%s", colorNormal, colorRedBold);
        printf("%s says: %s\n", player->get_name(), s->text);

        if (s->id == NPC_Ask_do_you_know_item)
        {
            InventoryElement * el = select_element(player->inventory);
            if (el)
            {
                player->ask(s, el);
            }
        }
        else
        {
            if (player->say(s))
            {
                current_npc = nullptr;
            }
        }

        printf("%s%s", colorNormal, colorGreenBold);
    }
}

void talk()
{
    if (!current_npc)
    {
        printf("Who do you want to talk to?\n");
        current_npc = dynamic_cast<Npc *>(select_element(npcs));
        sentences->enable_all();
        player->start_conversation(current_npc);
    }
    if (current_npc)
    {
        Sentence * s = dynamic_cast<Sentence *>(select_list_element(sentences));

        printf("%s%s", colorNormal, colorRedBold);
        printf("%s says: %s\n", player->get_name(), s->text);

        if (player->say(s))
        {
            current_npc = nullptr;
        }

        printf("%s%s", colorNormal, colorGreenBold);
    }
}

void hunt()
{
    InventoryElement * el = select_element(animals);
    //   plants->enable_all();
    if (!el)
        return;
    player->inventory->add(el);
    animals->remove(el);
    printf("animal: %s hunted to inventory\n", el->get_name());
}

void play()
{
    help();
    while (1)
    {
        char c = wait_key('>');

        switch (c)
        {
            case 't':
                test();
                break;
            case 'c':
                craft();
                break;
            case 's':
                show();
                break;
            case 'e':
                return;
            case 27:
                printf("%s", clrscr);
                break;
            case '?':
                help();
                break;
            case '&':
                change_clock();
                break;
            case 'h':
                harvest_plant();
                break;
            case 'o':
                sow_plant();
                break;
            case 'u':
                hunt();
                break;
            case 'f':
                find_new();
                break;
            case '#':
                talk();
                break;
            case '@':
                ask();
                break;
            case 'p':
                pickup();
                break;
            case 'd':
                drop();
                break;
            case '1':
                use();
                break;
        }
        game_time->update_time(1);
        plants->tick();
        animals->tick();
        npcs->tick();
    }
}

struct termios old_stdin, stdin_tty;
void set_terminal()
{
    setbuf(stdout, nullptr);
    printf("%s", clrscr);
    tcgetattr(0, &old_stdin);
    stdin_tty = old_stdin;
    stdin_tty.c_lflag &= ~(ECHO | ICANON | ECHOE | ISIG);
    stdin_tty.c_cc[VMIN] = 1;
    stdin_tty.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &stdin_tty);

    tcflush(0, TCIFLUSH);
    ioctl(0, TCXONC, 1);
}

int main()
{
    set_terminal();

    srandom(time(nullptr));
    init_elements();
    game_time = new Game_time;

    elements = new InvList("elements");
    plants = new InvList("plants");
    animals = new InvList("animals");
    npcs = new InvList("npcs");

    for (int i = 0; i < 5; i++)
        add_new_element();

    for (int i = 0; i < 2; i++)
    {
        add_new_plant();
    }

    for (int i = 0; i < 2; i++)
    {
        add_new_animal();
    }

    for (int i = 0; i < 1; i++)
    {
        add_new_npc();
    }
    player = new Player(0);

    init_sentences();
    init_questions();

    callback_daily = daily_call;
    play();

    tcsetattr(0, TCSANOW, &old_stdin);
    return 0;
}
