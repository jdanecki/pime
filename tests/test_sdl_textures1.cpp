#include <limits.h>
#include <dirent.h>
#include <stdio.h>

// #include "../core/tiles.h"

#include "../client-common/window.h"

#include "../client-common/texture.h"

int tile_size = 32;

int window_size = tile_size * CHUNK_SIZE;
int world_size = window_size * WORLD_SIZE;
int screen_tiles = window_size / tile_size + 2;
int screen_chunks = screen_tiles / CHUNK_SIZE + 2;

struct tilexy
{
    int tile;
    int x;
    int y;
};

struct tile
{
    int tile;
};

typedef struct tile chunk_table[CHUNK_SIZE][CHUNK_SIZE];
SDL_Texture * player_texture;

class chunk
{
  public:
    chunk_table table;
    chunk()
    {
        for (int ty = 0; ty < CHUNK_SIZE; ++ty)
        {
            for (int tx = 0; tx < CHUNK_SIZE; ++tx)
            {
                table[ty][tx].tile = rand() % TILE_TEXTURES;
            }
        }
    }
};

chunk * world_table[WORLD_SIZE][WORLD_SIZE];
class chunk_position;

class camera
{
  public:
    int x;
    int y;
    int size;
    int get_first_tile_x();
    int get_first_tile_y();
    int get_last_tile_x();
    int get_last_tile_y();
    camera();
    void draw();
    void show();
    void init();
};

camera::camera()
{
    init();
}

void camera::init()
{
    size = tile_size;
    x = 128 * CHUNK_SIZE * size - 8 * size;
    y = 128 * CHUNK_SIZE * size - 8 * size;
}
void camera::show()
{
    printf("camera: x:%d y:%d size:%d\n", x, y, size);
    int chunk_x = x / (CHUNK_SIZE * size);
    int chunk_y = y / (CHUNK_SIZE * size);
    int tile_x = (x - chunk_x * CHUNK_SIZE * size) / size;
    int tile_y = (y - chunk_y * CHUNK_SIZE * size) / size;
    printf("camera: chunk:[%d][%d] tile:[%d][%d]\n", chunk_x, chunk_y, tile_x, tile_y);
}

int camera::get_first_tile_x()
{
    return x / size;
}
int camera::get_first_tile_y()
{
    return y / size;
}
int camera::get_last_tile_x()
{
    return (x + window_size) / size + 1;
}
int camera::get_last_tile_y()
{
    return (y + window_size) / size + 1;
}

camera camera;

class world_position
{
    const char * name;

  public:
    int x;
    int y;
    chunk_position get_chunk_position();
    chunk_position get_chunk_position_ofs(int dx, int dy);
    void move_up();
    void move_down();
    void move_left();
    void move_right();
    void show();
    void draw(int ofs_x, int ofs_y, int size);
    void camera_draw(int ofs_x, int ofs_y, int size);
    world_position(const char * name);
    world_position(int x, int y);
};

class chunk_position
{
  public:
    int chunk_x;
    int chunk_y;
    int tile_x;
    int tile_y;

    world_position get_world_position();
};

chunk_position world_position::get_chunk_position()
{
    return {x / CHUNK_SIZE, y / CHUNK_SIZE, x % CHUNK_SIZE, y % CHUNK_SIZE};
}

chunk_position world_position::get_chunk_position_ofs(int dx, int dy)
{
    int nx = x + dx;
    int ny = y + dy;
    return {nx / CHUNK_SIZE, ny / CHUNK_SIZE, nx % CHUNK_SIZE, ny % CHUNK_SIZE};
}
void world_position::move_up()
{
    if (y > 0)
        y--;
}
void world_position::move_down()
{
    if (y < world_size - window_size)
        y++;
}
void world_position::move_left()
{
    if (x > 0)
        x--;
}
void world_position::move_right()
{
    if (x < world_size - window_size)
        x++;
}
world_position chunk_position::get_world_position()
{
    return {chunk_x * CHUNK_SIZE + tile_x, chunk_y * CHUNK_SIZE + tile_y};
}

void world_position::show()
{
    chunk_position cp = get_chunk_position();
    printf("world_position: %s[%d][%d]\n", name, x, y);
    printf("chunk_position: [%d][%d][%d][%d]\n", cp.chunk_x, cp.chunk_y, cp.tile_x, cp.tile_y);
}

void world_position::draw(int ofs_x, int ofs_y, int size)
{
    for (int ty = 0; ty < CHUNK_SIZE; ++ty)
    {
        for (int tx = 0; tx < CHUNK_SIZE; ++tx)
        {
            chunk_position cp = get_chunk_position_ofs(tx, ty);
            chunk * chunk = world_table[cp.chunk_y][cp.chunk_x];
            int tile = chunk->table[cp.tile_y][cp.tile_x].tile;
            SDL_Texture * texture = tiles_textures[tile];
            SDL_Rect img_rect = {ofs_x + tx * size, ofs_y + ty * size, size, size};
            SDL_RenderCopy(renderer, texture, NULL, &img_rect);
        }
    }
}

void world_position::camera_draw(int ofs_x, int ofs_y, int size)
{
    for (int ty = 0; ty < CHUNK_SIZE; ++ty)
    {
        for (int tx = 0; tx < CHUNK_SIZE; ++tx)
        {
            chunk_position cp = get_chunk_position_ofs(tx, ty);
            chunk * chunk = world_table[cp.chunk_y][cp.chunk_x];
            int tile = chunk->table[cp.tile_y][cp.tile_x].tile;
            SDL_Texture * texture = tiles_textures[tile];

            SDL_Rect img_rect = {ofs_x + tx * size - camera.x, ofs_y + ty * size - camera.y, size, size};

            SDL_RenderCopy(renderer, texture, NULL, &img_rect);
        }
    }
}
world_position::world_position(const char * name)
{
    this->name = name;
    x = 128 * CHUNK_SIZE - 8;
    y = 128 * CHUNK_SIZE - 8;
}

world_position::world_position(int x, int y)
{
    this->x = x;
    this->y = y;
}

world_position left("left");
world_position right1("right1");
world_position right2("right2");
world_position right3("right3");
world_position right4("right4");

void camera::draw()
{
    SDL_Rect clip = {0, 0, window_size, window_size};
    SDL_RenderSetClipRect(renderer, &clip);
    for (int ty = get_first_tile_y(); ty < get_last_tile_y(); ++ty)
    {
        for (int tx = get_first_tile_x(); tx < get_last_tile_x(); ++tx)
        {
            int chunk_x = tx / CHUNK_SIZE;
            int chunk_y = ty / CHUNK_SIZE;
            if (chunk_x < 0 || chunk_x >= world_size / CHUNK_SIZE || chunk_y < 0 || chunk_y >= world_size / CHUNK_SIZE)
                continue;

            chunk * chunk = world_table[chunk_y][chunk_x];

            int tile_x = tx - chunk_x * CHUNK_SIZE;
            int tile_y = ty - chunk_y * CHUNK_SIZE;

            int tile = chunk->table[tile_y][tile_x].tile;
            SDL_Texture * texture = tiles_textures[tile];
            SDL_Rect img_rect = {tx * size - x, ty * size - y, size, size};
            SDL_RenderCopy(renderer, texture, NULL, &img_rect);
        }
    }
    SDL_RenderSetClipRect(renderer, NULL);
}

bool key_pressed(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
            return true;
        case SDLK_UP:
            right1.move_up();
            break;
        case SDLK_DOWN:
            right1.move_down();
            break;
        case SDLK_LEFT:
            right1.move_left();
            break;
        case SDLK_RIGHT:
            right1.move_right();
            break;

        case SDLK_w:
            left.move_up();
            break;
        case SDLK_s:
            left.move_down();
            break;
        case SDLK_a:
            left.move_left();
            break;
        case SDLK_d:
            left.move_right();
            break;

        case SDLK_KP_8:
            if (camera.y > 0)
                camera.y--;
            break;
        case SDLK_KP_2:
            if (camera.y < world_size - window_size)
                camera.y++;
            break;
        case SDLK_KP_4:
            if (camera.x > 0)
                camera.x--;
            break;
        case SDLK_KP_6:
            if (camera.x < world_size - window_size)
                camera.x++;
            break;
        case SDLK_KP_5:
            camera.init();
            break;
        case SDLK_PAGEDOWN:
            camera.size *= 2;
            if (camera.size > window_size)
            {
                camera.size = window_size;
            }
            break;
        case SDLK_PAGEUP:
            camera.size /= 2;
            if (camera.size < 1)
            {
                camera.size = 1;
            }
            break;
        default:
            printf("key %d\n", key);
            break;
    }

    right2.x = right1.x + CHUNK_SIZE;
    right2.y = right1.y;

    right3.x = right1.x;
    right3.y = right1.y + CHUNK_SIZE;

    right4.x = right1.x + CHUNK_SIZE;
    right4.y = right1.y + CHUNK_SIZE;

    left.show();
    right1.show();
    right2.show();
    right3.show();
    right4.show();
    camera.show();
    return false;
}

void mouse_pressed(SDL_MouseButtonEvent & event)
{
    printf("mouse x: %d y: %d button=%d\n", event.x, event.y, event.button);
}

bool handle_events()
{
    SDL_Event event;

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

            if (key_pressed(key))
                return true;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_pressed(event.button);
        }
    }

    return false;
}

void draw()
{
    camera.draw();

    left.draw(window_size + 20, 0, tile_size);

    right1.draw(2 * window_size + 40, 0, tile_size / 2);
    right2.draw(2 * window_size + window_size / 2 + 60, 0, tile_size / 2);
    right3.draw(2 * window_size + 40, window_size / 2 + 20, tile_size / 2);
    right4.draw(2 * window_size + window_size / 2 + 60, window_size / 2 + 20, tile_size / 2);

    //    SDL_Rect img_rect = {mx * tile_size, py * tile_size, tile_size, tile_size};
    //  SDL_RenderCopy(renderer, player_texture, NULL, &img_rect);
    for (int tx = 0; tx < CHUNK_SIZE; ++tx)
    {
        SDL_Texture * texture = tiles_textures[tx % TILE_TEXTURES];
        SDL_Rect img_rect = {tx * tile_size, (1 + CHUNK_SIZE) * tile_size, tile_size, tile_size};
        SDL_RenderCopy(renderer, texture, NULL, &img_rect);
    }
}

int main()
{
    if (init_window("test textures1", 1700, 600))
        return 1;

    int i = 0;
    tiles_textures = new SDL_Texture *[TILE_TEXTURES];
    tiles_textures_count = add_textures_from_dir(tiles_textures, i, "textures/game_tiles");

    //   player_texture = load_texture("textures/player.png");
    for (int i = 0; i < WORLD_SIZE; ++i)
    {
        for (int j = 0; j < WORLD_SIZE; ++j)
        {
            world_table[i][j] = new chunk();
        }
    }
    for (;;)
    {
        clear_window();

        if (handle_events())
            break;

        draw();

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }
    return 0;
}
