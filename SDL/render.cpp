#include "main.h"
#include "networking.h"
#include "texture.h"
#include "window.h"
#include <SDL2/SDL_render.h>
#include <cstddef>

// TODO move it
int active_hotbar = 0;
int width;
int tx;
int game_size;
int tile_dungeon_size;
int request_delay = 0;

char text[300];

// TODO remove that when not necessary
extern NetClient * client;

void draw_texts()
{
    int ty = 10;

    sprintf(text, "Hunger=%d Thirst=%d %d %d", player->hunger, player->thirst, player->map_x, player->map_y);
    write_text(tx, ty, text, (player->hunger < 100 || player->thirst < 100) ? Red : White, 15, 30);
    ty += 25;

    InventoryElement * item = get_item_at_ppos(player);
    if (item)
    {

        char * t = player->get_el_description(item);
        if (t)
        {
            write_text(tx, ty, t, White, 15, 30);
            delete[] t;
            ty += 25;
            int count = 0;
            Property ** props = item->get_properties(&count);
            if (props)
            {
                char buf[64];
                for (int i = 0; i < count; i++)
                {
                    sprintf(buf, "%s: %u", props[i]->name.str, props[i]->value);
                    write_text(tx, ty, buf, White, 15, 30);
                    ty += 25;
                }
                delete props;
            }
        }
        else
        {
            t = new char[256];
            sprintf(t, "It looks like %s", item->get_class_name());
            write_text(tx, ty, t, White, 15, 30);
            Class_id el_cid = item->get_cid();
            ty += 25;
            if (el_cid == Class_Element)
            {
                sprintf(t, "it has %s form", item->get_form_name());
                write_text(tx, ty, t, White, 15, 30);
                ty += 25;
            }
            if (el_cid != Class_Ingredient && el_cid != Class_Product)
                write_text(tx, ty, "I don't know what it's exactly", White, 15, 30);

            delete[] t;
        }
    }
}

#if 0
int wait_for_chunk;

void draw_maps()
{
    unsigned int * pixels;
    int pitch, x, y;

    SDL_LockTexture(map, NULL, (void **)&pixels, &pitch);
    bool sent_request = false;

    int start_x = player->map_x - 5;
    int start_y = player->map_y - 5;

    for (y = start_y; y < start_y + 10; y++)
    {
        for (x = start_x; x < start_x + 10; x++)
        {
            if (x < 0)
                break;
            if (y < 0)
                break;
            if (x >= WORLD_SIZE)
                break;
            if (y >= WORLD_SIZE)
                break;

            chunk * chunk = world_table[y][x];
            if (chunk)
            {
                int tile = get_tile_at(x, y, 0, 0);
                BaseElement * b = get_base_element(tile);
                unsigned long c = 0xff000000 | (b->color.r) | (b->color.g << 8) | (b->color.b << 16);
                pixels[y * WORLD_SIZE + x] = c;
            }
            else
            {
                pixels[y * WORLD_SIZE + x] = 0xff303030;
                if (!wait_for_chunk)
                {
                    if (!sent_request)
                    {
                        printf("request_chunk: %d %d\n", x, y);
                        send_packet_request_chunk(client, x, y);
                        sent_request = true;
                    }
                    wait_for_chunk = 50;
                }
                else
                    wait_for_chunk--;
            }
        }
    }

    //    unsigned int p = pixels[player->map_y * WORLD_SIZE + player->map_x];
    for (y = 0; y < 3; y++)
        for (x = 0; x < 3; x++)
        {
            int py = player->map_y + y - 1;
            int px = player->map_x + x - 1;
            if (py >= 0 && py < WORLD_SIZE && px >= 0 && px < WORLD_SIZE)
                pixels[py * WORLD_SIZE + px] = 0xffffffff;
        }
    SDL_UnlockTexture(map);

    SDL_Rect window_rec;
    //  if (window_height > 650)
    {
        window_rec.w = WORLD_SIZE;
        window_rec.h = WORLD_SIZE;
    }
    /*else
    {
        window_rec.w = 0;
        window_rec.h = 0;
    }*/
    window_rec.x = width + 10;
    window_rec.y = 200; // window_height - WORLD_SIZE - STATUS_LINES;

    SDL_RenderCopy(renderer, map, NULL, &window_rec);
}
#endif

bool draw_terrain()
{
    width = window_width - PANEL_WINDOW;
    tx = width + 10;

    if (width < window_height)
    {
        game_size = width;
        tile_dungeon_size = width / (CHUNK_SIZE);
    }
    else
    {
        game_size = window_height;
        tile_dungeon_size = window_height / (CHUNK_SIZE);
    }

    // render terrain
    if (world_table[player->map_y][player->map_x])
    {
        request_delay = 0;
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                SDL_Rect img_rect = {x * tile_dungeon_size, y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
                // enum game_tiles tile = get_tile_at(player.map_x, player.map_y, x, y);
                int tile = get_tile_at(player->map_x, player->map_y, x, y);
                SDL_Texture * texture = tiles_textures[tile % tiles_textures_count];
                SDL_SetTextureColorMod(texture, get_base_element(tile)->color.r, get_base_element(tile)->color.g, get_base_element(tile)->color.b);
                SDL_RenderCopy(renderer, texture, NULL, &img_rect);
            }
        }
    }
    else
    {
        if (request_delay == 0)
        {
            send_packet_request_chunk(client, player->map_x, player->map_y);
            request_delay = 20;
        }
        else
        {
            request_delay--;
        }
        print_status(1, "chunk not loaded");
        return false;
    }
    chunk * c = world_table[player->map_y][player->map_x];
    if (c)
    {
        ListElement * el = c->objects.head;
        while (el)
        {
            InventoryElement * o = (el->el);
            Renderable * r = dynamic_cast<Renderable *>(o);
            if (o && r)
            {
                SDL_Rect img_rect = {o->get_x() * tile_dungeon_size, o->get_y() * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
                r->render(&img_rect);
            }
            else
            {
                printf("unrenderable\n");
            }

            el = el->next;
        }
    }
    return true;
}

void draw_players()
{
    // render players
    for (int i = 0; i < PLAYER_NUM; i++)
    {
        if (players[i] && player->map_x == players[i]->map_x && player->map_y == players[i]->map_y)
        {
            SDL_Rect img_rect = {players[i]->x * tile_dungeon_size, players[i]->y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
            if (players[i]->going_right)
                SDL_RenderCopy(renderer, Texture.player, NULL, &img_rect);
            else
                SDL_RenderCopyEx(renderer, Texture.player, NULL, &img_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
        }
    }
    // render GUI
    int icon_size = game_size / 10;
    if (player->running)
    {
        SDL_Rect running_icon_rect = {(int)(game_size - (icon_size * 1.1)), 0, icon_size, icon_size};
        SDL_RenderCopy(renderer, Texture.run_icon, NULL, &running_icon_rect);
    }
    if (player->sneaking)
    {
        SDL_Rect sneaking_icon_rect = {(int)(game_size - (icon_size * 1.1)), 0, icon_size, icon_size};
        SDL_RenderCopy(renderer, Texture.sneak_icon, NULL, &sneaking_icon_rect);
    }
}

void draw_npc()
{
    SDL_Rect img_rect = {5 * tile_dungeon_size, 5 * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
    static int tick = 0;
    static int dir = 1;
    int side;
    tick++;
    if (!(tick % 50))
    {
        dir *= -1;
    }

    side = dir > 0 ? 0 : 1;
    if (side)
        SDL_RenderCopy(renderer, npc_texture, NULL, &img_rect);
    else
        SDL_RenderCopyEx(renderer, npc_texture, NULL, &img_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
}

void draw()
{
    if (draw_terrain())
    {
        draw_players();
        draw_npc();
        draw_texts();
        //  draw_maps();
    }
    if (status_line[0] != ' ')
    {
        write_text(5, window_height - 64, status_line, White, 15, 30);
    }

    if (status_line2[0] != ' ')
        write_text(5, window_height - 32, status_line2, White, 15, 30);

    if (current_menu)
        current_menu->show();
}
