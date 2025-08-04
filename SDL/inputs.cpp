#include "inputs.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <cstdio>

#include "../core/player.h"
#include "../core/tiles.h"
#include "../core/packet_types.h"
#include "dialog/d_craft.h"
#include "dialog/d_hotbar.h"
#include "menu.h"
#include "networking.h"
#include "player_actions.h"
#include "window.h"

extern Player * player;
extern int active_hotbar;
extern int auto_explore;
extern DHotbar hotbar;

int last_frame_press = 0;
Uint64 last_time = 0;

// TODO cleanup this
void update_window_size()
{
    int tile_size;
    int width;
    SDL_GetWindowSize(main_window, &window_width, &window_height);

    width = window_width - PANEL_WINDOW;

    if (width < window_height)
    {
        tile_size = width / (CHUNK_SIZE);
    }
    else
    {
        tile_size = window_height / (CHUNK_SIZE);
    }
    if (tile_size < 32)
        tile_size = 32;

    SDL_SetWindowSize(main_window, (tile_size * CHUNK_SIZE) + PANEL_WINDOW, tile_size * CHUNK_SIZE + STATUS_LINES);
    SDL_GetWindowSize(main_window, &window_width, &window_height);
}

void key_pressed(int key)
{
    if (d_craft.show == false && menu_interact(key))
        return;

    switch (key)
    {
        case SDLK_v:
            sprintf(status_line, " ");
            break;

        /*case SDLK_SEMICOLON:
        {
            InventoryElement * el = player.hotbar[active_hotbar];
            if (el)
                el->show(true);
            InventoryElement ** at_ppos = get_item_at_ppos(&player);

            if (at_ppos)
            {
                el = *at_ppos;
                if (el)
                    el->show(true);
            }
            Being ** b_at_ppos = get_being_at_ppos(&player);
            if (b_at_ppos)
            {
                Being * b = *b_at_ppos;
                if (b)
                    b->show();
            }
            Plant ** p_at_ppos = get_plant_at_ppos(&player);
            if (p_at_ppos)
            {
                Plant * p = *p_at_ppos;
                if (p)
                    p->show();
            }
            Animal ** a_at_ppos = get_animal_at_ppos(&player);
            if (a_at_ppos)
            {
                Animal * a = *a_at_ppos;
                if (a)
                    a->show();
            }
            Object ** o_at_ppos = get_object_at_ppos(&player);
            if (o_at_ppos)
            {
                Object * o = *o_at_ppos;
                if (o)
                    o->show();
            }
        }
        break;

        case SDLK_f:
        {
            InventoryElement * el = player.hotbar[active_hotbar];
            if (el)
            {
                Edible * edible = el->get_edible();
                if (edible)
                {
                    player.thirst+=edible->irrigation;
                    player.hunger+=edible->caloric;
                    player.inventory->remove(el);
                    player.hotbar[active_hotbar]=NULL;
                    sprintf(status_line, "eat");
                    status_code = 1;
                    if (edible->poison)
                    {
                        player.thirst-=edible->poison*10;
                        player.hunger-=edible->poison*10;
                        sprintf(status_line, "eat: GOT POISONED");
                    }
                }
                else
                {
                    sprintf(status_line, "eat: not food");
                    status_code = 0;
                }
            }

            break;
        }*/
        case SDLK_F11:
            update_window_size();
            break;
        case SDLK_1:
            active_hotbar = 0;
            break;
        case SDLK_2:
            active_hotbar = 1;
            break;
        case SDLK_3:
            active_hotbar = 2;
            break;
        case SDLK_4:
            active_hotbar = 3;
            break;
        case SDLK_5:
            active_hotbar = 4;
            break;
        case SDLK_6:
            active_hotbar = 5;
            break;
        case SDLK_7:
            active_hotbar = 6;
            break;
        case SDLK_8:
            active_hotbar = 7;
            break;
        case SDLK_9:
            active_hotbar = 8;
            break;
        case SDLK_0:
            active_hotbar = 9;
            break;

        case SDLK_q:
            put_element();
            break;

        case SDLK_BACKQUOTE:
            active_hotbar--;
            if (active_hotbar == -1)
                active_hotbar = 9;
            break;
        case SDLK_TAB:
            active_hotbar++;
            if (active_hotbar == 10)
                active_hotbar = 0;
            break;

        case SDLK_MINUS:
            player->craftbar[active_hotbar] = 0;
            break;
        case SDLK_EQUALS:
            if (player->hotbar[active_hotbar])
                player->craftbar[active_hotbar] = 1;
            break;

            /* case SDLK_F5:
             {
                 auto_explore ^= 1;
                 break;
             }*/
        case SDLK_F1:
        {
            InventoryElement * item = get_item_at_ppos(player);
            if (item)
                item->show();
            else
                printf("nothing to show\n");
            break;
        }
        case SDLK_F2:
            server_action_tile(SERVER_SHOW_ITEM, player->map_x, player->map_y, player->x, player->y);
            break;

        case SDLK_F3:
            if (world_table[player->map_y][player->map_x])
                world_table[player->map_y][player->map_x]->show();
            break;
        case SDLK_F4:
            server_action_tile(SERVER_SHOW_CHUNK, player->map_x, player->map_y, player->x, player->y);
            break;

        case SDLK_F5:
            trace_network ^= true;
            break;
        case SDLK_F6:
            server_action_tile(SERVER_TRACE_NETWORK, player->map_x, player->map_y, player->x, player->y);
            break;

        case SDLK_RETURN:
            use_tile(player->map_x, player->map_y, player->x, player->y);
            break;
        case SDLK_c:
            d_craft.show ^= 1;
            break;
        case SDLK_ESCAPE:
            d_craft.show = 0;
            break;
    }
}

void mouse_pressed(SDL_MouseButtonEvent & event)
{
    /*   switch (event.button.button)
    {
        case 1: break;
        case 2: break;
        case 3: break;
    }
 */
    InventoryElement * el = player->hotbar[active_hotbar];
    int x = 0;
    int y = 0;

    SDL_GetMouseState(&x, &y);
    hotbar.press(x, y, event.button == 3 ? true : false);
    if (d_craft.show)
    {
        d_craft.press(x, y);
        return;
    }

    int tile_dungeon_size;
    int width = window_width - PANEL_WINDOW;

    if (width < window_height)
    {
        tile_dungeon_size = width / (CHUNK_SIZE);
    }
    else
    {
        tile_dungeon_size = window_height / (CHUNK_SIZE);
    }

    int tile_x = x / tile_dungeon_size;
    int tile_y = y / tile_dungeon_size;

    if (tile_x < CHUNK_SIZE && tile_y < CHUNK_SIZE)
    {
        if (el)
        {
            // if (el->use(player->map_x, player->map_y, tile_x, tile_y)) break;
            // if (plant_with_seed(el, player->map_x, player->map_y, tile_x, tile_y)) break;
            //  TODO
            /*if ((Element *)el && (Element *)el->get_base() && ((Element *)el)->get_base()->id == ID_WATER)
            {
                if (Plant ** pp = get_plant_at(player->map_x, player->map_y, tile_x, tile_y))
                {
                    if (Plant * p = *pp)
                    {
                        p->water += 100;
                        player->inventory->remove(el);
                        player->hotbar[active_hotbar]=NULL;
                        free(el);
                        break;
                    }
                }
            }*/
        }
        use_tile(player->map_x, player->map_y, tile_x, tile_y);
    }
    printf("mouse %d,%d %d %d,%d\n", event.x, event.y, event.button, tile_x, tile_y);
}

Uint64 handle_keyboard_state(const Uint8 * keys)
{
    Uint64 current_time = SDL_GetTicks64();
    Uint64 time_period = 0;
    if (keys[SDL_SCANCODE_LSHIFT])
    {
        player->sneaking = 1;
        time_period = 200;
    }
    else
    {
        player->sneaking = 0;
        if (keys[SDL_SCANCODE_LCTRL] && player->hunger && player->thirst)
        {
            player->running = 1;
            time_period = 50;
        }
        else
        {
            player->running = 0;
            time_period = 100;
        }
    }

    if (current_menu == NULL && ((current_time - last_time > time_period) || !(last_frame_press)))
    {
        if (keys[SDL_SCANCODE_DOWN])
        {
            send_packet_move(client, 0, 1);
            last_frame_press = 1;
        }
        else if (keys[SDL_SCANCODE_UP])
        {
            send_packet_move(client, 0, -1);
            last_frame_press = 1;
        }
        if (keys[SDL_SCANCODE_RIGHT])
        {
            player->going_right = 1;
            send_packet_move(client, 1, 0);
            last_frame_press = 1;
        }
        else if (keys[SDL_SCANCODE_LEFT])
        {
            player->going_right = 0;
            send_packet_move(client, -1, 0);
            last_frame_press = 1;
        }
        if (last_frame_press)
        {
            return SDL_GetTicks64();
        }
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_LEFT])
    {
        print_status(0, " ");
        print_status(1, " ");
        last_frame_press = 1;
        return 0;
    }

    last_frame_press = 0;
    return 0;
}

bool handle_SDL_events()
{
    int ww = 0, wh = 0;
    SDL_Event event;
    // keyboard handling for not move
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            return true;
        };
        if (event.type == SDL_KEYDOWN)
        {
            int key = event.key.keysym.sym;

            key_pressed(key);
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            // i3 window manager sends these events if window is not floated
            if (ww != event.window.data1 && wh != event.window.data2)
            {
                printf("window event: resizing to %d, %d\n", event.window.data1, event.window.data2);
                update_window_size();
                ww = event.window.data1;
                wh = event.window.data2;
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_pressed(event.button);
        }
    }
    const Uint8 * currentKeyState = SDL_GetKeyboardState(NULL);
    Uint64 tmp = handle_keyboard_state(currentKeyState);
    if (tmp > 0)
    {
        last_time = tmp;
    }
    return 0;
}
