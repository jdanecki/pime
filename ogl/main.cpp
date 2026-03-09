/*
    pime-ogl - an OpenGL implementation of a pime client
    Copyright (C) 2026 Piotr Danecki <i3riced@mailfence.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <SDL3/SDL.h>
#include <cmath>
#include <cstdio>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include "../net/net.h"
#include "ogl.h"
#include "main.h"

OGL_World * ogl_world;
OGL_Text * ogl_text;

OGL_Camera cam;

bool mouse_grabbed = true;
float cam_x_lt = cam.x;
float cam_z_lt = cam.z;

SDL_Window * window;
SDL_GLContext ctx;
int window_width;
int window_height;
float fps;
int render_distance = 4;

typedef struct PIME_gamepad
{
    double x1, x2;
    double y1, y2;
    double rt;
    bool a;

    bool * get_button_by_id(Uint8 id)
    {
        switch (id)
        {
            case 0:
                return &this->a;
        }
        return NULL;
    }

    void button_down(Uint8 id)
    {
        if (bool * button = get_button_by_id(id))
        {
            *button = 1;
        }
    }

    void button_up(Uint8 id)
    {
        if (bool * button = get_button_by_id(id))
        {
            *button = 0;
        }
    }

    double * get_axis_by_id(Uint8 id)
    {
        switch (id)
        {
            case 0:
                return &this->x1;
            case 1:
                return &this->y1;
            case 2:
                return &this->x2;
            case 3:
                return &this->y2;
            case 5:
                return &this->rt;
        }
        return NULL;
    }
} PIME_gamepad;
PIME_gamepad gp;

bool show_all_chunks = false;
std::vector<size_t> tmp_inventory; // FIXME: once packet changes are pushed, change to server behaviour
size_t my_id;

std::vector<SDL_JoystickID> joystick_ids;

void print_status(int, char const *, ...) {};
int CONSOLE_LOG(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return 0;
};

void update_hotbar() {};

void get_forward_vector(float yaw, float * x, float * z)
{
    float yaw_rad = -yaw * M_PI / 180.0f;
    *x = -sinf(yaw_rad);
    *z = -cosf(yaw_rad);
}

InventoryElement * raycast()
{
    float dx, dy, dz;
    cam.get_forward_vector(&dx, &dy, &dz);
    dx *= 0.1;
    dy *= 0.1;
    dz *= 0.1;
    float cx = cam.x;
    float cy = cam.y;
    float cz = cam.z;
    for (int i = 0; i < 50; i++)
    { // 5 units / 0.1 (max vector length)
        cx += dx;
        cy += dy;
        cz += dz;
        OGL_Chunk * ch = NULL;
        if (!(ch = ogl_world->ogl_chunks[(int)floor(cz / CHUNK_SIZE)][(int)floor(cx / CHUNK_SIZE)]))
        {
            continue;
        }
        for (auto [_, inv_element] : ch->elements)
        {
            if (!inv_element || inv_element->uid == my_id || dynamic_cast<Player *>(inv_element))
                continue;
            if (abs(cx - inv_element->location.get_world_x()) < inv_element->dimensions.width.value / 2)
                if (abs(cz - inv_element->location.get_world_y()) < inv_element->dimensions.length.value / 2)
                    if (OGL_Node * onode = dynamic_cast<OGL_Node *>(inv_element))
                        if (abs(cy - onode->ogl_position.y) < onode->ogl_dimensions.height / 2)
                        {
                            return inv_element;
                        }
        }
    }
    return NULL;
}

GLuint load_texture(const char * filename)
{
    glEnable(GL_TEXTURE_2D);
    SDL_Surface * surface = SDL_LoadPNG(filename);
    surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_FlipSurface(surface, SDL_FLIP_VERTICAL);

    unsigned int texture;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    PFNGLGENERATETEXTUREMIPMAPPROC glGenerateMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)SDL_GL_GetProcAddress("glGenerateMipmap");
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
    SDL_DestroySurface(surface);
    return texture;
}

chunk * check_chunk(int cx, int cy)
{
    if (cx < 0 || cy < 0 || cx >= WORLD_SIZE || cy >= WORLD_SIZE)
        return nullptr;

    chunk * ch = world_table[cy][cx];
    if (!ch)
    {
        send_packet_request_chunk(cx, cy);
        loaded_chunks[cy][cx] = CHUNK_LOADING;
        return nullptr;
    }
    else
    {
        loaded_chunks[cy][cx] = CHUNK_LOADED;
    }
    return ch;
}

void pickup_pointing()
{
    if (InventoryElement * el = raycast())
    {
        send_packet_pickup(el->uid);
        tmp_inventory.push_back(el->uid);
    }
}

void drop_item()
{
    if (tmp_inventory.size() == 0)
        return;
    send_packet_drop(tmp_inventory.back());
    tmp_inventory.pop_back();
}

void handle_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if ((e.type == SDL_EVENT_MOUSE_MOTION && mouse_grabbed))
            cam.rotate_by(e.motion.xrel * 0.5f, -e.motion.yrel * 0.5f);

        if (e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
        {
            // printf("%d pressed\n", e.gbutton.button);
            gp.button_down(e.gbutton.button);
            if (e.gbutton.button == 2) // x
            {
                drop_item();
            }
        }
        if (e.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
        {
            gp.button_up(e.gbutton.button);
        }

        if (e.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
        {
            // printf("%d %d\n", e.gaxis.axis, e.gaxis.value);
            if (double * to_modify = gp.get_axis_by_id(e.gaxis.axis))
            {
                *to_modify = (double)e.gaxis.value;
            }
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == 1)
        {
            SDL_SetWindowRelativeMouseMode(window, true);
            mouse_grabbed = true;
            pickup_pointing();
        }

        if (e.type == SDL_EVENT_WINDOW_RESIZED)
        {
            SDL_GetWindowSize(window, &window_width, &window_height);
        }
        if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            disconnect();
            exit(0);
        }
        if (e.type == SDL_EVENT_KEY_DOWN)
        {
            switch (e.key.scancode)
            {
                case SDL_SCANCODE_ESCAPE:
                    SDL_SetWindowRelativeMouseMode(window, false);
                    mouse_grabbed = false;
                    break;
                case SDL_SCANCODE_F1:
                    show_all_chunks ^= 1;
                    break;
                case SDL_SCANCODE_EQUALS:
                    render_distance += 1;
                    break;
                case SDL_SCANCODE_MINUS:
                    render_distance -= 1;
                    if (render_distance < 0)
                        render_distance = 0;
                    break;
                case SDL_SCANCODE_Q:
                    drop_item();
                    break;
            }
        }
    }
}

bool check_step(float dx, float dz)
{
    float new_x = cam.x + dx;
    float new_z = cam.z + dz;

    OGL_Chunk * ch = NULL;
    if (!(ch = ogl_world->ogl_chunks[(int)floor(new_z / CHUNK_SIZE)][(int)floor(new_x / CHUNK_SIZE)]))
    {
        return false;
    }
    for (auto [_, inv_element] : ch->elements)
    {
        if (!inv_element || inv_element->uid == my_id)
            continue;
        if (abs(new_x - inv_element->location.get_world_x()) < inv_element->dimensions.width.value / 2 + 0.25)
            if (abs(new_z - inv_element->location.get_world_y()) < inv_element->dimensions.length.value / 2 + 0.25)
            {
                return false;
            }
    }
    return true;
};

void move_check_step(float dx, float dz)
{
    if (check_step(dx, dz))
    {
        cam.x += dx;
        cam.z += dz;
    }
}

void handle_keyboard_state(Uint64 dt)
{
    const bool * keyboard_state = SDL_GetKeyboardState(NULL);
    float speed_multi = 0.1f;
    if (keyboard_state[SDL_SCANCODE_LCTRL])
        speed_multi = 1.0f;

    speed_multi *= dt;
    speed_multi /= 16;

    if (keyboard_state[SDL_SCANCODE_SPACE] && cam.y == 1.5)
        cam.vy = 0.25;
    cam.vy -= 0.025;
    cam.y += cam.vy;
    if (cam.y < 1.5)
    {
        cam.y = 1.5;
        cam.vy = 0;
    }
    if (keyboard_state[SDL_SCANCODE_A])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        move_check_step(z * speed_multi, -x * speed_multi);
    }
    if (keyboard_state[SDL_SCANCODE_D])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        move_check_step(-z * speed_multi, x * speed_multi);
    }
    if (keyboard_state[SDL_SCANCODE_W])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        move_check_step(x * speed_multi, z * speed_multi);
    }
    if (keyboard_state[SDL_SCANCODE_S])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        move_check_step(-x * speed_multi, -z * speed_multi);
    }

    if (abs(cam.x - cam_x_lt) || abs(cam.x - cam_x_lt))
        send_packet_move(cam.x - cam_x_lt, cam.z - cam_z_lt);
}

void init_ogl()
{
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, ctx);
    SDL_GL_SetSwapInterval(-1);
    if (!ctx)
        exit(3);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void load_textures()
{
    DIR * dir = opendir("./textures/game_tiles");
    if (dir == NULL)
    {
        perror("Can't open textures directory");
        exit(4);
    }
    closedir(dir);

    struct dirent ** namelist;
    int n = scandir("./textures/game_tiles", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        char path[300];
        while (n--)
        {
            if (namelist[n]->d_name[0] == '.' && (namelist[n]->d_name[1] == '\0' || (namelist[n]->d_name[1] == '.' && namelist[n]->d_name[2] == '\0')))
            {
                continue;
            }
            sprintf(path, "%s/%s", "./textures/game_tiles", namelist[n]->d_name);
            CONSOLE_LOG("adding texture: %s\n", path);
            load_texture(path);
            free(namelist[n]);
        }
        free(namelist);
    }
}

void draw()
{
    cam.begin_camera(window);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    int chunk_x = cam.x / CHUNK_SIZE;
    int chunk_z = cam.z / CHUNK_SIZE;

    for (int chi = chunk_x - render_distance; chi <= chunk_x + render_distance; chi++)
    {
        for (int chj = chunk_z - render_distance; chj <= chunk_z + render_distance; chj++)
        {
            check_chunk(chi, chj);
        }
    }
    if (show_all_chunks)
        ogl_world->render(0, 0, WORLD_SIZE, WORLD_SIZE);
    else
        ogl_world->render(chunk_x - render_distance, chunk_z - render_distance, chunk_x + render_distance, chunk_z + render_distance);

    glColor4f(1, 1, 1, 1);
    char buf[512] = {
        0,
    };
    float _x, _y, _z;
    cam.get_forward_vector(&_x, &_y, &_z);
    snprintf(buf, 256, "x: %.2f y: %.2f z: %.2f\nmap_x: %d, map_y: %d\nyaw: %.4f pitch: %.4f\nFacing %s\nRender_distance: %d\nfps: %.2f\nforward vec: %f, %f, %f", cam.x, cam.y, cam.z,
        (int)(cam.x / CHUNK_SIZE), (int)(cam.z / CHUNK_SIZE), cam.yaw, cam.pitch, cam.get_direction_string(), render_distance, fps, _x, _y, _z);
    ogl_text->draw_text(buf, 0, 0, 2, window_width, window_height);
    ogl_text->setup_2d_projection(window_width, window_height);
    {
        int wh2 = window_height / 2;
        int ww2 = window_width / 2;
        glColor4f(1, 1, 1, 1);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glVertex2f(ww2 - 2, wh2 - 2);
        glVertex2f(ww2 + 2, wh2 - 2);
        glVertex2f(ww2 + 2, wh2 + 2);
        glVertex2f(ww2 - 2, wh2 + 2);
        glEnd();
    }
    ogl_text->restore_3d_projection();
    glDisable(GL_TEXTURE_2D);
    SDL_GL_SwapWindow(window);
}

void handle_arguments(int argc, char * argv[])
{
    if (argc < 2)
    {
        CONSOLE_LOG("usage: ./pime_ogl <ip> [port]\n");
        CONSOLE_LOG("using localhost 127.0.0.1:1234\n");
        ip = "127.0.0.1";
    }
    else
    {
        ip = argv[1];
    }
    if (argc < 3)
    {
        port = "1234";
    }
    else
    {
        port = argv[2];
    }
}

void init_sdl()
{
    window = SDL_CreateWindow("pime_ogl", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        CONSOLE_LOG("Problem with SDL window creation\n");
        exit(2);
    }
    SDL_SetWindowRelativeMouseMode(window, true);
}

void init_world()
{
    ogl_world = new OGL_World;
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 0; j < WORLD_SIZE; j++)
            world_table[i][j] = NULL;
}

void handle_gp_state()
{
    cam_x_lt = cam.x;
    cam_z_lt = cam.z;
    if (abs(gp.x2) > 500) // deadzone
    {
        cam.rotate_by(gp.x2 * 4 / 32768, 0);
    }

    if (abs(gp.y2) > 500) // deadzone
    {
        cam.rotate_by(0, -gp.y2 * 4 / 32768);
    }

    if (abs(gp.x1) > 500 || abs(gp.y1) > 500) // deadzone
    {
        float yaw_rad = cam.yaw * M_PI / 180.0f;
        double c = cos(yaw_rad);
        double s = sin(yaw_rad);
        // Rotating a vector
        // new x = old x * cos(rotation) - old y * sin(rotation)
        // new y = old x * sin(rotation) + old y * cos(rotation)
        double mx = gp.x1 * c / 32768 - gp.y1 * s / 32768;
        double my = gp.x1 * s / 32768 + gp.y1 * c / 32768;
        move_check_step(mx / 4, my / 4);
    }

    if (gp.a && cam.y == 1.5)
        cam.vy = 0.25;

    if (gp.rt > 16384)
    {
        pickup_pointing();
    }
}

void handle_gamepads()
{
    int count = 0;
    SDL_JoystickID * ids = SDL_GetGamepads(&count);

    for (int i = 0; i < count; i++)
    {
        if (std::find(joystick_ids.begin(), joystick_ids.end(), ids[i]) != joystick_ids.end())
        {
            continue;
        }
        SDL_OpenGamepad(ids[i]);
        // printf("Opening new gamepad %d!\n", ids[i]);
        joystick_ids.push_back(ids[i]);
    }
}

int main(int argc, char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
    handle_arguments(argc, argv);
    if (!init_networking())
    {
        CONSOLE_LOG("Problem with server connection\n");
        return 1;
    }
    init_world();
    init_sdl();
    init_ogl();
    load_textures();
    ogl_text = new OGL_Text("font.png", 8, 12, 16, 32);

    Uint64 dt = 0;
    Uint64 t;
    for (;;)
    {
        t = SDL_GetTicks();
        handle_gamepads();
        handle_events();
        handle_gp_state();
        handle_keyboard_state(dt);
        network_tick();
        draw();
        dt = SDL_GetTicks() - t;
        fps = 1000.0 / dt;
    }
}
