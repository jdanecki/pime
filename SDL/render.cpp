#include "main.h"
#include "networking.h"
#include "npc.h"
#include "text.h"
#include "texture.h"
#include "window.h"
#include <SDL2/SDL_render.h>
#include <cstddef>
#include <cstdio>
#include "dialog/d_craft.h"
#include <cassert>

// TODO move it
int active_hotbar = 0;
int width;
int tx;
int game_size;
int tile_dungeon_size;

int left_chunk_x;
int right_chunk_x;
int top_chunk_y;
int bottom_chunk_y;

DHotbar hotbar;

char text[300];

// TODO remove that when not necessary
extern NetClient * client;

void draw_texts()
{
    int ty = 10;

    sprintf(text, "Hunger=%d Thirst=%d", player->hunger, player->thirst);
    write_text(tx, ty, text, (player->hunger < 100 || player->thirst < 100) ? Red : White, 15, 30);

    sprintf(text, "%d,%d/%d,%d", left_chunk_x, top_chunk_y, right_chunk_x, bottom_chunk_y);
    write_text(tx, window_height - 150, text, White, 15, 30);

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
            char buf[64];
            if (props)
            {
                for (int i = 0; i < count; i++)
                {
                    sprintf(buf, "%s: %u", props[i]->name.str, props[i]->value);
                    write_text(tx, ty, buf, White, 15, 30);
                    ty += 25;
                }
                delete props;
            }
            Class_id el_cid = item->get_cid();
            if (el_cid == Class_Element)
            {
                sprintf(buf, "it has %s form", item->get_form_name());
                write_text(tx, ty, buf, White, 15, 30);
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

    int player_world_x = get_world_x(player->location);
    int player_world_y = get_world_y(player->location);

    int left_top_world_x = player_world_x - CHUNK_SIZE / 2;
    int left_top_world_y = player_world_y - CHUNK_SIZE / 2;

    left_chunk_x = left_top_world_x / CHUNK_SIZE;
    right_chunk_x = (left_top_world_x + CHUNK_SIZE - 1) / CHUNK_SIZE;

    top_chunk_y = left_top_world_y / CHUNK_SIZE;
    bottom_chunk_y = (left_top_world_y + CHUNK_SIZE - 1) / CHUNK_SIZE;

    for (int cy = top_chunk_y; cy <= bottom_chunk_y; ++cy)
    {
        for (int cx = left_chunk_x; cx <= right_chunk_x; ++cx)
        {
            if (cx < 0 || cy < 0 || cx >= WORLD_SIZE || cy >= WORLD_SIZE)
                return false;

            chunk * ch = world_table[cy][cx];
            if (!ch)
            {
                if (loaded_chunks[cy][cx] == CHUNK_NOT_LOADED)
                {
                    send_packet_request_chunk(client, cx, cy);
                    loaded_chunks[cy][cx] = CHUNK_LOADING;
                    return false;
                }
                else
                {
                    printf("waiting for chunk %d %d\n", cx, cy);
                    return false;
                }
            }
            else
            {
                loaded_chunks[cy][cx] = CHUNK_LOADED;
            }
            for (int ty = 0; ty < CHUNK_SIZE; ++ty)
            {
                for (int tx = 0; tx < CHUNK_SIZE; ++tx)
                {
                    int world_x = cx * CHUNK_SIZE + tx;
                    int world_y = cy * CHUNK_SIZE + ty;

                    int screen_x = world_x - left_top_world_x;
                    int screen_y = world_y - left_top_world_y;

                    if (screen_x >= 0 && screen_x < CHUNK_SIZE && screen_y >= 0 && screen_y < CHUNK_SIZE)
                    {
                        int tile = ch->table[ty][tx].tile;

                        SDL_Rect img_rect = {screen_x * tile_dungeon_size, screen_y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};

                        SDL_Texture * texture = tiles_textures[tile % tiles_textures_count];
                        SDL_SetTextureColorMod(texture, get_base_element(tile)->color.r, get_base_element(tile)->color.g, get_base_element(tile)->color.b);
                        SDL_RenderCopy(renderer, texture, NULL, &img_rect);
                    }
                }
            }
        }
    }

    //  printf("%d,%d -> %d,%d\n", left_chunk_x, top_chunk_y, right_chunk_x, bottom_chunk_y);
    for (int cy = top_chunk_y; cy <= bottom_chunk_y; ++cy)
    {
        for (int cx = left_chunk_x; cx <= right_chunk_x; ++cx)
        {
            if (cx < 0 || cy < 0 || cx >= WORLD_SIZE || cy >= WORLD_SIZE)
                continue;

            chunk * ch = world_table[cy][cx];
            if (!ch)
                continue;

            ListElement * el = ch->objects.head;
            while (el)
            {
                InventoryElement * o = (el->el);
                Renderable * r = dynamic_cast<Renderable *>(o);
                if (o && r)
                {
                    int obj_world_x = cx * CHUNK_SIZE + o->get_x();
                    int obj_world_y = cy * CHUNK_SIZE + o->get_y();

                    int screen_x = obj_world_x - left_top_world_x;
                    int screen_y = obj_world_y - left_top_world_y;

                    if (screen_x >= 0 && screen_x < CHUNK_SIZE && screen_y >= 0 && screen_y < CHUNK_SIZE)
                    {
                        SDL_Rect img_rect = {screen_x * tile_dungeon_size, screen_y * tile_dungeon_size, tile_dungeon_size, tile_dungeon_size};
                        r->render(&img_rect);
                    }
                }
                el = el->next;
            }
        }
    }
    return true;
}

void draw_players()
{
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
    // FIXME
    if (!current_npc)
        return;
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
        SDL_RenderCopy(renderer, current_npc->get_texture(), NULL, &img_rect);
    else
        SDL_RenderCopyEx(renderer, current_npc->get_texture(), NULL, &img_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
}

void draw_dialogs()
{
    if (d_craft.show)
    {
        d_craft.draw(renderer);
    }
    hotbar.draw(renderer);
}

void draw()
{
    hotbar.update();
    d_craft.update();

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
    draw_dialogs();
}
