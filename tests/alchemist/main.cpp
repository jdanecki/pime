#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "../../core/alchemist/elements/element.h"
#include "../../core/alchemist/elements/animal.h"
#include "../../core/alchemist/elements/plant.h"
#include "../../core/alchemist/el_list.h"
#include "../../core/alchemist/npc_talk.h"
#include "../../core/npc.h"
#include "../../core/world.h"

#include "game_time.h"
#include "plants.h"
#include "show_list.h"

#include "../../client-common/ui/console.h"

ElementsList * elements;
ElementsList * animals;
ElementsList * npcs;

chunk * current_chunk;

Player * player;

Npc * current_npc;

void (*callback_daily)();

void daily_call()
{
    printf("daily call\n");
    plants->tick();
    animals->tick();
}

void help()
{
    printf("%sESC/e/?- clear screen/Exit/Help\n", colorWhite);

//    printf("c - Craft\n");
    printf("h - Harvest plant\n");
    printf("o - sOw plant\n");
    printf("u - hUnt animal\n");
    //printf("1 - use item from inventory\n");

    printf("s - Show\n");
    printf("& - change clock\n");

    printf("f - Find\n");
    printf("p - Pickup up element\n");
    printf("d - Drop element\n");

    printf("# - conversation\n");
    printf("@ - ask questions\n");

  //  printf("t - Test\n");
}

void show_description()
{
    printf("Which item do you want to describe?\n");
    InventoryElement * el = select_element(&player->inventory);
    if (!el)
        return;
    char * des = player->get_el_description(el);
    if (des)
        printf("%s\n", des);
    else
        printf("It looks like %s, it has %s form, but I don't know what it's exactly\n", el->get_class_name(), el->get_form_name());
}

void show()
{
    printf("%se/E - elements (details off/on)\n", colorCyan);
    printf("i/I - inventory (details off/on)\n");
    printf("p/P - plants (details off/on)\n");
    printf("a/A - animals (details off/on)\n");
    printf("n/N - npcs/player (details off/on)\n");
    printf("d - item description\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key("show");
    switch (c)
    {
        case 'i':
        case 'I':
            player->inventory.show(c == 'I');
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
    Element * el = new Element(new BaseElement(Form_solid, 0));
    elements->add(el);
    printf("new Element %s found\n", el->get_name());
}

void add_new_animal()
{
    Animal * el = new Animal(new BaseAnimal(0));
    animals->add(el);
    printf("new Animal %s found\n", el->get_name());
}

void add_new_plant()
{
    Plant * p = new Plant(new BasePlant(0));
    plants->add(p);
    printf("new Plant %s found\n", p->get_name());
}

size_t npc_count;
void add_new_npc()
{
    ItemLocation location = ItemLocation::center();
    Npc * el = new Npc(npc_count++, location);
    npcs->add(el);
    current_chunk->add_object(el, 8.5, 8.5);
    printf("new NPC %s found\n", el->get_name());
}

void find_new()
{
    printf("%se - Element\n", colorCyan);
    printf("p - Plant\n");
    printf("a - Animal\n");
    printf("n - Npc\n");
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key("find");
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

    char c = wait_key("clock");
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

void pickup()
{
    printf("Which item do you want to pick up?\n");
    InventoryElement * el = select_element(elements);
    if (!el)
        return;
    player->inventory.add(el);
    elements->remove(el);
    printf("%s added to inventory\n", el->get_name());
}

void drop()
{
    printf("Which item do you want to drop?\n");
    InventoryElement * el = select_element(&player->inventory);
    if (!el)
        return;
    printf("%s dropped from inventory\n", el->get_name());
    elements->add(el);
    player->inventory.remove(el);
}

void ask_say(char c)
{ // # say
  // @ ask
    if (!current_npc)
    {
        printf("Who do you want to talk to?\n");
        current_npc = dynamic_cast<Npc *>(select_element(npcs));
        if (c == '@')
            questions->enable_all();
        else
            sentences->enable_all();
    }
    if (current_npc)
    {
        if (c == '@')
        {
            if (!player->inventory.nr_elements)
                questions->disable(NPC_Ask_do_you_know_inv_item);
            else
            {
                questions->enable(NPC_Ask_do_you_know_inv_item);
            }
        }
        Sentence * s;
        if (c == '@')
            s = dynamic_cast<Sentence *>(select_list_element(questions));
        else
            s = dynamic_cast<Sentence *>(select_list_element(sentences));

        printf("%s%s", colorNormal, colorRedBold);
        printf("%s says: %s\n", player->get_name(), s->text);

        InventoryElement * el = nullptr;
        if (c == '@')
        {
            if (s->id == NPC_Ask_do_you_know_inv_item)
            {
                el = select_element(&player->inventory);
                if (!el) {
                    printf("%s%s", colorNormal, colorGreenBold);
                    return;
                }
            }
        }
        if (!player->conversation(s, el))
            current_npc = nullptr;
        printf("%s%s", colorNormal, colorGreenBold);
    }
}

void hunt()
{
    InventoryElement * el = select_element(animals);
    //   plants->enable_all();
    if (!el)
        return;
    player->inventory.add(el);
    animals->remove(el);
    printf("animal: %s hunted to inventory\n", el->get_name());
}

void play()
{
    help();
    while (1)
    {
        char c = wait_key("main");

        switch (c)
        {
/*          case 't':
                test();
                break;
            case 'c':
                craft_entry();
                break;*/
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
            case '@':
                ask_say(c);
                break;
            case 'p':
                pickup();
                break;
            case 'd':
                drop();
                break;
/*          case '1':
                use();
                break; */
        }
        game_time->update_time(1);
        //  plants->tick();
        //  animals->tick();
        //  npcs->tick();
        if (status_line[0] != ' ') {
            printf("%s\n", status_line);
            status_line[0]=0;
        }
        if (status_line2[0] != ' ') {
            printf("%s\n", status_line2);
            status_line2[0]=0;
        }
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

    game_time = new Game_time;

    elements = new ElementsList("elements");
    plants = new ElementsList("plants");
    animals = new ElementsList("animals");
    npcs = new ElementsList("npcs");

    chunk_table table;
    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
            table[y][x].tile = random_range(0, 16);

    current_chunk = new chunk(128, 128);
    world_table[128][128] = current_chunk;
    memcpy(world_table[128][128]->table, &table[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(int));

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
    ItemLocation location=ItemLocation::center();
    player = new Player(0, SerializableCString("player"), location, 100, 100, 100);

    init_sentences();
    init_questions();
    init_answers();

    callback_daily = daily_call;
    play();

    tcsetattr(0, TCSANOW, &old_stdin);
    return 0;
}
