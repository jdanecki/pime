#include <stdarg.h>

#include "../core/alchemist/el_list.h"
#include "networking.h"
#include "chunk.h"
#include "console.h"
#include "generator/generator.h"
#include "key_handlers.h"
#include "npc_server.h"

void generate()
{
    create_regions();

    for (int i = 0; i < all_base_elements_count; i++)
    {
        Form f;
        int id;

        if (i < terrains_count)
        {
            f = (Form)terrains[i]->form;
            id = terrains[i]->id;
        }
        else
        {
            f = (Form)(random_range(1, 4));
            id = i;
        }
        ListElement * entry = new BaseListElement(new BaseElement(f, id));
        base_elements.add(entry);
    }
    for (int i = 0; i < all_plants_count; i++)
    {
        ListElement * entry = new BaseListElement(new BasePlant(all_plants[i]->id));
        base_plants.add(entry);
    }
    for (int i = 0; i < all_animals_count; i++)
    {
        ListElement * entry = new BaseListElement(new BaseAnimal(all_animals[i]->id));
        base_animals.add(entry);
    }
    load_chunks();
}

int main()
{
    int ret = 0;
    ncurses_init();
    print_status(0, "pime_enet, press F1 for help\n");
    CONSOLE_LOG("Copyright (C) 2025 Piotr Danecki <i3riced@mailfence.com>\n");
    CONSOLE_LOG("Copyright (C) 2025 Jacek Danecki\n");
    CONSOLE_LOG("This program comes with ABSOLUTELY NO WARRANTY.\n");
    CONSOLE_LOG("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    CONSOLE_LOG("See LICENSE file\n");
    //   trace_network = 1;

    srand(0);
    if (!init_networking())
    {
        ret = 1;
        goto end_server;
    }

    generate();
    create_players();
    create_npcs();
    
    ENetEvent event;

    while (enet_host_service(server, &event, 20) >= 0)
    {
        ncurses_tick();
        if (handle_pressed())
            break;
        handle_net_event(&event);
    }
    enet_host_destroy(server);

end_server:
    delwin(in_w);
    delwin(out_w);
    endwin();
    if (ret)
        fprintf(stderr, "Error: %d\n", ret);
    return ret;
}
