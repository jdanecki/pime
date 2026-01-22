#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "render.h"
#include "text.h"

#include "../dialog/d_craft.h"
#include "../dialog/d_hotbar.h"

#include "playerUI.h"
#include "net.h"
// #include "npc.h"
#include "text.h"
#include "texture.h"
#include "window.h"
#include "alchemist2d.h"
// #include "being2d.h"
#include "../menu/menu.h"

extern Backend_Texture map;

extern int active_hotbar;
extern int auto_explore;

unsigned int left_chunk_x;
unsigned int right_chunk_x;
unsigned int top_chunk_y;
unsigned int bottom_chunk_y;

DHotbar hotbar;

char text[300];

// TODO remove that when not necessary
extern NetClient * client;
void draw_texts()
{
    int tx = window_width - PANEL_WINDOW + 10;
    int ty = 10;
    Backend_Rect r(tx - 10, 0, PANEL_WINDOW, window_height - 64);
    Backend_Draw_Fill_Rectangle(r, Backend_Color{10, 10, 200, 255});

    sprintf(text, "Hunger=%d Thirst=%d", player->hunger, player->thirst);
    write_text(tx, ty, text, (player->hunger < 100 || player->thirst < 100) ? Red : White, 15, 30);
    int32_t pl_ch_x = player->location.chunk.map_x;
    int32_t pl_ch_y = player->location.chunk.map_y;
    float px = player->location.chunk.x;
    float py = player->location.chunk.y;
    chunk * ch = world_table[pl_ch_y][pl_ch_x];
    int tile = ch->table[(int)(py + 0.5)][(int)(px + 0.5)].tile;
    BaseElement * base = get_base_element(tile % tiles_textures_count);

    sprintf(text, "%s@[%d,%d][%0.1f,%0.1f]:id=%ld f=%c", player->get_name(), pl_ch_x, pl_ch_y, px, py, base->uid, (Form_name[base->form])[0]);
    write_text(tx, window_height - 100, text, White, 15, 30);

    InventoryElement * item = get_item_at_ppos(player);
    if (item)
    {
        Class_id el_cid = item->get_cid();
        char * t = player->get_el_description(item);
        if (!t)
        {
            t = new char[256];
            sprintf(t, "It looks like %s %s ", el_cid == Class_Element ? item->get_form_name() : " ", item->get_class_name());
        }
        print_status(1, t);
        delete[] t;
        // item->location.show();

        // if (player->checked_element == item->uid)
        {
            ty += 30;
            int count = 0;
            Property ** props = item->get_properties(&count);
            char buf[64];
            if (props)
            {
                for (int i = 0; i < count; i++)
                {
                    sprintf(buf, "%s: %u", props[i]->name.str, props[i]->value);
                    write_text(tx, ty, buf, White, 15, 30);
                    ty += 27;
                }
                delete props;
            }
        }
    }
    InventoryElement * el = player->hotbar[active_hotbar];
    if (el)
    {
        if (el->get_cid() == Class_Product)
        {
            Product * pr = static_cast<Product *>(el);
            if (pr)
            {
                sprintf(text, "%s -> %s", el->get_name(), product_action_name[pr->actions[0]]);
            }
        }
        else
        {
            sprintf(text, "%s (%s)", el->get_name(), el->get_class_name());
        }
        write_text(window_width - PANEL_WINDOW + 10, hotbar.rect.r.y + hotbar.rect.get_h(), text, Cyan, 13, 25);
    }
}

#if 0
int wait_for_chunk;
Backend_Pixels pixels; 
void draw_maps()
{    
    int x, y;
    if (!pixels.pixels)
        pixels = Backend_Allocate_Pixels(WORLD_SIZE, WORLD_SIZE);
    else Backend_Map_Pixels(pixels);

    bool sent_request = false;

    int start_x = player->location.chunk.map_x - 5;
    int start_y = player->location.chunk.map_y - 5;

    for (y = start_y; y < start_y + 10; y++)
    {
        for (x = start_x; x < start_x + 10; x++)
        {            
            chunk * chunk = world_table[y][x];
            unsigned int * pixel=(unsigned int*)(pixels.pixels);
            if (chunk)
            {                       
                unsigned char g=chunk->beings.nr_elements *10;
                unsigned char b=0;
                unsigned char r=chunk->objects.nr_elements *10;
                unsigned long c = 0xff000000 | (r) | (g << 8) | (b << 16);
                pixel[y * WORLD_SIZE + x] = c;                
            }
            else
            {                
                pixel[y * WORLD_SIZE + x] = 0xff303030;
                if (!wait_for_chunk)
                {
                    if (!sent_request)
                    {
                       CONSOLE_LOG("request_chunk: %d %d\n", x, y);
                        send_packet_request_chunk(x, y);
                        sent_request = true;
                        //pixel[y * WORLD_SIZE + x] = 0xffff0000;
                    } else {
                       // pixel[y * WORLD_SIZE + x] = 0xff0000ff;
                    }
                    wait_for_chunk = 50;
                }
                else
                {
                    //pixel[y * WORLD_SIZE + x] = 0xff00ffff;
                    wait_for_chunk--;
                }
                
            }
        }
    }

    //    unsigned int p = pixels[player->map_y * WORLD_SIZE + player->map_x];
    for (y = 0; y < 3; y++)
        for (x = 0; x < 3; x++)
        {
            int px = player->location.chunk.map_x + x - 1;
            int py = player->location.chunk.map_y + y - 1;

            if (py >= 0 && py < WORLD_SIZE && px >= 0 && px < WORLD_SIZE)
            {
                unsigned int * pixel=(unsigned int*)(pixels.pixels);
                pixel[py * WORLD_SIZE + px] = 0xffffffff;
            }
        }
    Backend_Update_Texture_Pixels(pixels);

    Backend_Rect window_rec(window_width - PANEL_WINDOW + 280, window_height - WORLD_SIZE - STATUS_LINES -150,        WORLD_SIZE, WORLD_SIZE );
    Backend_Texture_Copy(pixels.texture, NULL, &window_rec);
}
#endif

void render_element(InventoryElement * o, float ltx, float lty)
{
    Renderable * r = dynamic_cast<Renderable *>(o);
    if (r)
    {
        float obj_world_x = o->location.get_world_x();
        float obj_world_y = o->location.get_world_y();

        float screen_x = obj_world_x - ltx;
        float screen_y = obj_world_y - lty;

        if (screen_x < CHUNK_SIZE && screen_y < CHUNK_SIZE)
        {
            r->render(screen_x * tile_size, screen_y * tile_size);
            //            CONSOLE_LOG("render %s@[%d,%d]\n", o->get_name(), screen_x, screen_y);
        }
    }
    else
    {
        CONSOLE_LOG("unrenderable %d\n", o->get_cid());
    }
}

bool draw_terrain()
{
    int size = tile_size;

    float player_world_x = player->location.chunk.map_x * CHUNK_SIZE * size + player->location.chunk.x * size;
    float player_world_y = player->location.chunk.map_y * CHUNK_SIZE * size + player->location.chunk.y * size;

    float left_top_world_x = player_world_x - CHUNK_SIZE / 2.0 * size;
    float left_top_world_y = player_world_y - CHUNK_SIZE / 2.0 * size;   
       
    int first_tile_x = left_top_world_x / size;
    int first_tile_y = left_top_world_y / size;
    int last_tile_x = (left_top_world_x + size * CHUNK_SIZE) / size + 1;
    int last_tile_y = (left_top_world_y + size * CHUNK_SIZE) / size + 1;
    
    
    Backend_Set_Clip(0, 0, size * CHUNK_SIZE, size * CHUNK_SIZE);

    for (int ty = first_tile_y; ty < last_tile_y; ++ty)
    {
        for (int tx = first_tile_x; tx < last_tile_x; ++tx)
        {
            int chunk_x = tx / CHUNK_SIZE;
            int chunk_y = ty / CHUNK_SIZE;  
            int tile_x = tx - chunk_x * CHUNK_SIZE;
            int tile_y = ty - chunk_y * CHUNK_SIZE;
            if (chunk_x < 0 || chunk_x >= WORLD_SIZE || chunk_y < 0 || chunk_y >= WORLD_SIZE)
                continue;
            chunk * chunk = check_chunk(chunk_x, chunk_y);
            if (!chunk) {
                Backend_Set_Clip(0, 0, 0, 0);
                return false;
            }
            
            int tile = chunk->table[tile_y][tile_x].tile;
            Backend_Rect img_rect(tx * size - left_top_world_x, ty * size - left_top_world_y, size, size);
            Backend_Texture texture = tiles_textures[tile % tiles_textures_count];
            BaseElement * base = get_base_element(tile % tiles_textures_count);
            if (base)
            {
                Backend_Color color{base->color.r, base->color.g, base->color.b, 255};
                switch (base->form)
                {
                    case Form_solid:
                        break;
                    case Form_liquid:
                        // b|=128;
                        break;
                    case Form_gas:
                        // SDL_SetTextureAlphaMod(texture, 128);
                        break;
                }
                Backend_Texture_Copy_With_Mask(texture, NULL, &img_rect, color, true);
            }
            else
                Backend_Texture_Copy(texture, NULL, &img_rect);
        }
    }
    Backend_Set_Clip(0, 0, 0, 0);


    /*  for (int i=0; i < tiles_textures_count; i++)
      {
          Backend_Texture texture = tiles_textures[i];
          Backend_Rect img_rect(i * tile_size, 16 * tile_size, tile_size, tile_size);
          SDL_RenderCopy(renderer, texture, NULL, &img_rect);
      }
  */
    int first_chunk_x = first_tile_x / CHUNK_SIZE;
    int first_chunk_y = first_tile_y / CHUNK_SIZE;
    int last_chunk_x = last_tile_x / CHUNK_SIZE;
    int last_chunk_y = last_tile_y / CHUNK_SIZE;
  
    player_world_x = player->location.get_world_x();
    player_world_y = player->location.get_world_y();

    left_top_world_x = player_world_x - CHUNK_SIZE / 2.0;
    left_top_world_y = player_world_y - CHUNK_SIZE / 2.0; 
  
   // CONSOLE_LOG("%d,%d -> %d,%d\n", first_chunk_x, first_chunk_y, last_chunk_x, last_chunk_y);
    for (unsigned int cy = first_chunk_y; cy <= last_chunk_y; ++cy)
    {
        for (unsigned int cx = first_chunk_x; cx <= last_chunk_x; ++cx)
        {
            if (cx >= WORLD_SIZE || cy >= WORLD_SIZE)
                continue;

            chunk * ch = world_table[cy][cx];
            if (!ch)
                continue;

            ReversedView revert = ch->objects.reversed();
            /*CONSOLE_LOG("cx=%d cy=%d head=%p next=%p prev=%p tail=%p elem=%d\n", cx, cy,
                revert.list->head, revert.list->head->next, revert.list->head->prev,
                revert.list->tail, revert.list->nr_elements);*/
            // revert.show();
            for (InventoryElement * o : revert)
            {
                if (!o)
                {
                    CONSOLE_LOG("null object in chunk\n");
                    return false;
                }
                if (o == player)
                    continue;
                render_element(o, left_top_world_x, left_top_world_y);
            }
        }
    }
  
    if (player && player->c_id == Class_Player) {
        render_element(player, left_top_world_x, left_top_world_y);
    }

    return true;
}

void draw_run_icons()
{
    int w = window_width - PANEL_WINDOW;
    int icon_size = w / 10;
    int x = (int)(w - (icon_size * 1.1));

    if (player->running)
    {
        Backend_Rect running_icon_rect(x, 0, icon_size, icon_size);
        Backend_Texture_Copy(Player_textures.run_icon, NULL, &running_icon_rect);
    }
    if (player->sneaking)
    {
        Backend_Rect sneaking_icon_rect(x, 0, icon_size, icon_size);
        Backend_Texture_Copy(Player_textures.sneak_icon, NULL, &sneaking_icon_rect);
    }
}

#if 0

void draw_npc()
{
    // FIXME
    if (!current_npc)
        return;
    Backend_Rect img_rect(5 * tile_size, 5 * tile_size, tile_size, tile_size);
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
#endif

void draw_dialogs()
{
    if (d_craft.show)
    {
        d_craft.update();
        d_craft.draw();
    }
    hotbar.update();
    hotbar.draw();
}

void draw_status()
{
    Backend_Rect r0(0, window_height - 64, window_width, 32);
    Backend_Draw_Fill_Rectangle(r0, Backend_Color{10, 100, 10, 255});
    Backend_Rect r1(0, window_height - 32, window_width, 32);
    Backend_Draw_Fill_Rectangle(r1, Backend_Color{10, 100, 10, 255});

    if (status_line[0] != ' ')
    {
        write_text(5, window_height - 64, status_line, White, 15, 30, false);
    }

    if (status_line2[0] != ' ')
    {
        write_text(5, window_height - 32, status_line2, White, 15, 30, false);
    }
}
int ui_updates;

void draw()
{
    Backend_Begin_Drawing();    
    ui_updates++;
    bool ret = draw_terrain();

   // if (ret && ui_updates > 3)
    {
        draw_run_icons();
        // draw_npc();
        draw_texts();

        // draw_maps();
        draw_status();
        ui_updates = 0;
    }
    
    draw_dialogs();

    if (current_menu)
        current_menu->show();

    Backend_Update_Screen();
    Backend_End_Drawing();
}
