#include <stdio.h>
#include <stdarg.h>

#include "../core/alchemist/el_list.h"
#include "player_server.h"
#include "world_server.h"
#include "places/places.h"
#include "networking.h"
#include "chunk.h"
#include "console.h"
#include "generator/generator.h"

// #define DEBUG_TIMEOUT 1

void print_help()
{
    CONSOLE_LOG("e - exit\n");
    CONSOLE_LOG("h - help\n");
    CONSOLE_LOG("c - clear\n");
    CONSOLE_LOG("p - players\n");
    CONSOLE_LOG("1 - show terrains types\n");
    CONSOLE_LOG("2 - show plants types\n");
    CONSOLE_LOG("3 - show animals types\n");
    CONSOLE_LOG("4 - show chunk\n");
    CONSOLE_LOG("5 - show base elements\n");
    CONSOLE_LOG("6 - show base plants\n");
    CONSOLE_LOG("7 - show base animals\n");
    CONSOLE_LOG("# - show keys\n");
}

char handle_command();
char handle_pressed(int pressed)
{
    if (pressed == 0)
        return 0;

    switch (pressed)
    {
        case KEY_BACKSPACE:
        case 8:
            if (strlen(in_buf))
            {
                in_buf[strlen(in_buf) - 1] = '\0';
            }
            break;
        case '\n':
        {
            CONSOLE_LOG("> %s\n", in_buf);
            char retval = handle_command();
            in_buf[0] = '\0';
            return retval;
        }
        case KEY_RESIZE:
            handle_resize(); // FALLTHROUGH
        case KEY_END:
            scrolling = true;
            history_up = buf_pos - win_h + 1;
            if (history_up < 0)
                history_up = 0;
            break;
        case KEY_HOME:
            scrolling = false;
            history_up = 0;
            break;
        case KEY_DOWN:
            scrolling = false;
            if (history_up < (buf_pos - 5))
                history_up += 5;
            break;
        case KEY_UP:
            scrolling = false;
            history_up -= 5;
            if (history_up < 0)
                history_up = 0;
            break;
        case '#':
            show_key ^= 1;
            break;
        default:
            char append[2] = {(char)pressed, 0};
            strcat(in_buf, append);
            break;
    }
    return 0;
}

char handle_command()
{
    if (!in_buf[0])
        return 0;
    switch (in_buf[0])
    {
        case 'e':
            return 1;
        case 'h':
            print_help();
            break;
        case 'c':
            clear_history();
            break;
        case 'p':
            show_players();
            break;
        default:
            CONSOLE_LOG("invalid command: %s\n", in_buf);
            CONSOLE_LOG("use \"help\" to show help\n");
            break;
        case '1':
            show_terrains();
            break;
        case '2':
            show_plants();
            break;
        case '3':
            show_animals();
            break;
        case '4':
            show_chunk();
            break;
        case '5':
            base_elements.show();
            break;
        case '6':
            base_plants.show();
            break;
        case '7':
            base_animals.show();
            break;
    }

    return 0;
}

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
            f = (Form)(1 + rand() % 3);
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
    ncurses_init();
    print_status(0, "pime_enet\n");
    CONSOLE_LOG("Copyright (C) 2025 Piotr Danecki <i3riced@mailfence.com>\n");
    CONSOLE_LOG("Copyright (C) 2025 Jacek Danecki\n");
    CONSOLE_LOG("This program comes with ABSOLUTELY NO WARRANTY.\n");
    CONSOLE_LOG("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    CONSOLE_LOG("See LICENSE file\n");
    //   trace_network = 1;

    srand(0);
    if (!init_networking())
        return 1;

    generate();
    create_players();

    ENetEvent event;
    char hostname[512] = {
        0,
    };

    while (enet_host_service(server, &event, 20) >= 0)
    {
        ncurses_tick();
        int ch = wgetch(in_w);
        if (show_key)
        {
            if (ch != ERR)
                CONSOLE_LOG("key=%d ", ch);
        }
        if (ch != ERR && handle_pressed(ch))
            break;
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                enet_address_get_host_ip(&event.peer->address, hostname, 512);
                CONSOLE_LOG("Client connected %s:%d\n", hostname, event.peer->address.port);
                unsigned short * port = new unsigned short;
                *port = event.peer->address.port;
                event.peer->data = (void *)port;
#if DEBUG_TIMEOUT
                enet_peer_timeout(event.peer, 5 * 60 * 1000, 5 * 60 * 1000, 5 * 60 * 1000); // 5 minutes
#endif
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                handle_packet(event.packet, event.peer);
                enet_packet_destroy(event.packet);
                send_updates();
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                Peer_id peer_id;
                peer_id.tag = Peer_id::Tag::Peer;
                peer_id.peer = event.peer;
                PlayerClient * pl = (PlayerClient *)players->find(&peer_id);
                enet_address_get_host_ip(&event.peer->address, hostname, 512);

                CONSOLE_LOG("player: %s from %s:%d disconnected\n", pl->player->get_name(), hostname, event.peer->address.port);
                destroy(pl->player);
                players->remove(pl);
                send_updates();
                event.peer->data = NULL;
                break;
            }
            default:
                // CONSOLE_LOG("time=%ld\n", get_time_ms());
                update();
                send_updates();
                break;
        }
    }
    delwin(in_w);
    delwin(out_w);
    endwin();

    enet_host_destroy(server);
    return 0;
}
