#include <SDL3/SDL.h>
#include <cmath>
#include <cstdio>
#include <dirent.h>
#include "../client-common/net.h"
#include "main.h"

OGL_Chunk * ogl_tiles[WORLD_SIZE][WORLD_SIZE];

OGL_Camera cam;

float cam_x_lt = cam.x;
float cam_z_lt = cam.z;

SDL_Window * window;
SDL_GLContext ctx;

size_t my_id;

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
    float yaw_rad = yaw * M_PI / 180.0f;
    *x = -sinf(yaw_rad);
    *z = -cosf(yaw_rad);
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
    return texture;
}

chunk * check_chunk(int cx, int cy)
{
    if (cx < 0 || cy < 0 || cx >= WORLD_SIZE || cy >= WORLD_SIZE)
        return nullptr;

    chunk * ch = world_table[cy][cx];
    if (!ch)
    {
        if (loaded_chunks[cy][cx] == CHUNK_NOT_LOADED)
        {
            send_packet_request_chunk(cx, cy);
            loaded_chunks[cy][cx] = CHUNK_LOADING;
            return nullptr;
        }
        else
        {
            // CONSOLE_LOG("waiting for chunk %d %d\n", cx, cy);
            return nullptr;
        }
    }
    else
    {
        loaded_chunks[cy][cx] = CHUNK_LOADED;
    }
    return ch;
}

void handle_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_MOUSE_MOTION)
        {
            cam.yaw -= e.motion.xrel * 0.5f;
            cam.pitch -= e.motion.yrel * 0.5f;
            if (cam.pitch > 90)
                cam.pitch = 90;
            if (cam.pitch < -90)
                cam.pitch = -90;
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == 1)
            SDL_SetWindowRelativeMouseMode(window, true);
        if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            disconnect();
            exit(0);
        }
        if (e.type == SDL_EVENT_WINDOW_RESIZED)
        {
            int w = e.window.data1, h = e.window.data2;
            glViewport(0, 0, w, (h > 0 ? h : 1));
        }
        if (e.type == SDL_EVENT_KEY_DOWN)
        {
            if (e.key.scancode == SDL_SCANCODE_ESCAPE)
                SDL_SetWindowRelativeMouseMode(window, false);
        }
    }
}

void handle_keyboard_state()
{
    const bool * keyboard_state = SDL_GetKeyboardState(NULL);
    float speed_multi = 0.1f;
    if (keyboard_state[SDL_SCANCODE_LCTRL])
        speed_multi = 1.0f;

    cam_x_lt = cam.x;
    cam_z_lt = cam.z;

    if (keyboard_state[SDL_SCANCODE_SPACE])
        cam.y += speed_multi;
    if (keyboard_state[SDL_SCANCODE_LSHIFT])
        cam.y -= speed_multi;
    if (keyboard_state[SDL_SCANCODE_A])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        cam.x += z * speed_multi;
        cam.z -= x * speed_multi;
    }
    if (keyboard_state[SDL_SCANCODE_D])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        cam.x -= z * speed_multi;
        cam.z += x * speed_multi;
    }
    if (keyboard_state[SDL_SCANCODE_W])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        cam.x += x * speed_multi;
        cam.z += z * speed_multi;
    }
    if (keyboard_state[SDL_SCANCODE_S])
    {
        float x, z;
        get_forward_vector(cam.yaw, &x, &z);
        cam.x -= x * speed_multi;
        cam.z -= z * speed_multi;
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
    // printf("cam_x=%f cam_x=%f chunk_x=%d chunk_z=%d\n", cam_x, cam_z, chunk_x, chunk_z);

    for (int chi = chunk_x - 2; chi <= chunk_x + 2; chi++)
    {
        for (int chj = chunk_z - 2; chj <= chunk_z + 2; chj++)
        {
            check_chunk(chi, chj);
        }
    }
    for (int chi = chunk_x - 10; chi <= chunk_x + 10; chi++)
    {
        for (int chj = chunk_z - 10; chj <= chunk_z + 10; chj++)
        {
            if (OGL_Chunk * ch = ogl_tiles[chj][chi])
            {
                ch->render();
            }
        }
    }

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

void init_world_table()
{
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 0; j < WORLD_SIZE; j++)
            world_table[i][j] = NULL;
}

int main(int argc, char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    handle_arguments(argc, argv);
    if (!init_networking())
    {
        CONSOLE_LOG("Problem with server connection\n");
        return 1;
    }
    init_world_table();
    init_sdl();
    init_ogl();
    load_textures();

    for (;;)
    {
        network_tick();
        handle_events();
        handle_keyboard_state();
        draw();
    }
}
