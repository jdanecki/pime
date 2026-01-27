#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>
#include "../client-common/net.h"

PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;

float cam_x = 128 * CHUNK_SIZE + 8;
float cam_y = 8;
float cam_z = 128 * CHUNK_SIZE + 8;

size_t my_id;

static void load_perspective(float fovy_deg, float aspect, float znear, float zfar)
{
    float fovy = fovy_deg * (float)M_PI / 180.0f;
    float f = 1.0f / tanf(fovy * 0.5f);
    float nf = 1.0f / (znear - zfar);

    // Column-major order for glMultMatrixf (OpenGL expects column-major)
    float m[16] = {f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (zfar + znear) * nf, -1, 0, 0, (2.0f * zfar * znear) * nf, 0};

    glMultMatrixf(m);
}

// Vertex structure
typedef struct
{
    float x, y, z;    // position
    float nx, ny, nz; // normal
    float u, v;       // texture coordinates
} Vertex;

void generate_cube_vertices(Vertex * vertices, float tx, float ty, float tz)
{
    int idx = 0;
    const float half = 0.5f; // Half the cube size

    // Top face - y = +half
    vertices[idx++] = (Vertex){tx - half, ty + half, tz + half, 0, 1, 0, 0, 0};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz + half, 0, 1, 0, 1, 0};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz - half, 0, 1, 0, 1, 1};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz - half, 0, 1, 0, 1, 1};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz - half, 0, 1, 0, 0, 1};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz + half, 0, 1, 0, 0, 0};

    // Bottom face - y = -half
    vertices[idx++] = (Vertex){tx - half, ty - half, tz - half, 0, -1, 0, 0, 0};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz - half, 0, -1, 0, 1, 0};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz + half, 0, -1, 0, 1, 1};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz + half, 0, -1, 0, 1, 1};
    vertices[idx++] = (Vertex){tx - half, ty - half, tz + half, 0, -1, 0, 0, 1};
    vertices[idx++] = (Vertex){tx - half, ty - half, tz - half, 0, -1, 0, 0, 0};

    // Front face - z = +half
    vertices[idx++] = (Vertex){tx - half, ty - half, tz + half, 0, 0, 1, 0, 0};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz + half, 0, 0, 1, 1, 0};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz + half, 0, 0, 1, 1, 1};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz + half, 0, 0, 1, 1, 1};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz + half, 0, 0, 1, 0, 1};
    vertices[idx++] = (Vertex){tx - half, ty - half, tz + half, 0, 0, 1, 0, 0};

    // Back face - z = -half
    vertices[idx++] = (Vertex){tx + half, ty - half, tz - half, 0, 0, -1, 0, 0};
    vertices[idx++] = (Vertex){tx - half, ty - half, tz - half, 0, 0, -1, 1, 0};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz - half, 0, 0, -1, 1, 1};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz - half, 0, 0, -1, 1, 1};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz - half, 0, 0, -1, 0, 1};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz - half, 0, 0, -1, 0, 0};

    // Right face - x = +half
    vertices[idx++] = (Vertex){tx + half, ty - half, tz + half, 1, 0, 0, 0, 0};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz - half, 1, 0, 0, 1, 0};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz - half, 1, 0, 0, 1, 1};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz - half, 1, 0, 0, 1, 1};
    vertices[idx++] = (Vertex){tx + half, ty + half, tz + half, 1, 0, 0, 0, 1};
    vertices[idx++] = (Vertex){tx + half, ty - half, tz + half, 1, 0, 0, 0, 0};

    // Left face - x = -half
    vertices[idx++] = (Vertex){tx - half, ty - half, tz - half, -1, 0, 0, 0, 0};
    vertices[idx++] = (Vertex){tx - half, ty - half, tz + half, -1, 0, 0, 1, 0};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz + half, -1, 0, 0, 1, 1};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz + half, -1, 0, 0, 1, 1};
    vertices[idx++] = (Vertex){tx - half, ty + half, tz - half, -1, 0, 0, 0, 1};
    vertices[idx++] = (Vertex){tx - half, ty - half, tz - half, -1, 0, 0, 0, 0};
}

GLuint vbo = 0;

void place_thing(GLuint texture, int r, int g, int b, Vertex * vertices, size_t vert_count)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f((double)r / 255, (double)g / 255, (double)b / 255);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO to use client-side memory

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].x);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].u);

    glDrawArrays(GL_TRIANGLES, 0, vert_count);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void place_plane(float tx, float ty, float tz, GLuint texture, int r, int g, int b)
{
    Vertex vertices[6];
    int idx = 0;
    vertices[idx++] = (Vertex){tx - 0.5f, ty + 0.5f, tz + 0.5f, 0, 1, 0, 0, 0};
    vertices[idx++] = (Vertex){tx + 0.5f, ty + 0.5f, tz + 0.5f, 0, 1, 0, 1, 0};
    vertices[idx++] = (Vertex){tx + 0.5f, ty + 0.5f, tz - 0.5f, 0, 1, 0, 1, 1};
    vertices[idx++] = (Vertex){tx + 0.5f, ty + 0.5f, tz - 0.5f, 0, 1, 0, 1, 1};
    vertices[idx++] = (Vertex){tx - 0.5f, ty + 0.5f, tz - 0.5f, 0, 1, 0, 0, 1};
    vertices[idx++] = (Vertex){tx - 0.5f, ty + 0.5f, tz + 0.5f, 0, 1, 0, 0, 0};
    place_thing(texture, r, g, b, vertices, 6);
}

void place_cube(float tx, float ty, float tz, GLuint texture, int r, int g, int b)
{
    Vertex vertices[36];
    generate_cube_vertices(vertices, tx, ty, tz);
    place_thing(texture, r, g, b, vertices, 36);
}

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

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    port = "1234";
    ip = "127.0.0.1";
    // ip = "192.168.0.3";

    if (!init_networking())
    {
        CONSOLE_LOG("Problem with server connection\n");
        return 1;
    }

    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 0; j < WORLD_SIZE; j++)
            world_table[i][j] = NULL;

    SDL_Window * win = SDL_CreateWindow("SDL3 OPENGL", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!win)
    {
        CONSOLE_LOG("Problem with SDL window creation\n");
        return 2;
    }

    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, ctx);
    SDL_GL_SetSwapInterval(-1);
    if (!ctx)
        return 3;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DIR * dir = opendir("./textures/game_tiles");
    if (dir == NULL)
    {
        perror("Can't open textures directory");
        return 4;
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

    bool running = true;
    SDL_Event e;

    float cam_x_lt = cam_x;
    float cam_z_lt = cam_z;

    float pitch = 0.0f;
    float yaw = 0.0f;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
    glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
    glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
    if (!glBindBuffer || !glGenBuffers || !glBufferData)
    {
        CONSOLE_LOG("Failed to load OpenGL functions!\n");
        return 5;
    }

    Vertex * all_vertices = (Vertex *)malloc(100 * 100 * 36 * sizeof(Vertex));
    int idx = 0;
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
        {
            generate_cube_vertices(&all_vertices[idx], j + 125 * CHUNK_SIZE, -4, i + 125 * CHUNK_SIZE);
            idx += 36;
        }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 100 * 100 * 36 * sizeof(Vertex), all_vertices, GL_STATIC_DRAW);
    free(all_vertices);

    while (running)
    {
        network_tick();
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
                yaw -= e.motion.xrel * 0.5f;
                pitch -= e.motion.yrel * 0.5f;
                if (pitch > 90)
                    pitch = 90;
                if (pitch < -90)
                    pitch = -90;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == 1)
                SDL_SetWindowRelativeMouseMode(win, true);
            if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                running = false;
            }
            if (e.type == SDL_EVENT_WINDOW_RESIZED)
            {
                int w = e.window.data1, h = e.window.data2;
                glViewport(0, 0, w, (h > 0 ? h : 1));
            }
            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                if (e.key.scancode == SDL_SCANCODE_ESCAPE)
                    SDL_SetWindowRelativeMouseMode(win, false);
            }
        }

        const bool * keyboard_state = SDL_GetKeyboardState(NULL);
        float speed_multi = 0.1f;
        if (keyboard_state[SDL_SCANCODE_LCTRL])
            speed_multi = 1.0f;

        cam_x_lt = cam_x;
        cam_z_lt = cam_z;

        if (keyboard_state[SDL_SCANCODE_SPACE])
            cam_y += speed_multi;
        if (keyboard_state[SDL_SCANCODE_LSHIFT])
            cam_y -= speed_multi;
        if (keyboard_state[SDL_SCANCODE_A])
        {
            float x, z;
            get_forward_vector(yaw, &x, &z);
            cam_x += z * speed_multi;
            cam_z -= x * speed_multi;
        }
        if (keyboard_state[SDL_SCANCODE_D])
        {
            float x, z;
            get_forward_vector(yaw, &x, &z);
            cam_x -= z * speed_multi;
            cam_z += x * speed_multi;
        }
        if (keyboard_state[SDL_SCANCODE_W])
        {
            float x, z;
            get_forward_vector(yaw, &x, &z);
            cam_x += x * speed_multi;
            cam_z += z * speed_multi;
        }
        if (keyboard_state[SDL_SCANCODE_S])
        {
            float x, z;
            get_forward_vector(yaw, &x, &z);
            cam_x -= x * speed_multi;
            cam_z -= z * speed_multi;
        }

        if (abs(cam_x - cam_x_lt) || abs(cam_x - cam_x_lt))
            send_packet_move(cam_x - cam_x_lt, cam_z - cam_z_lt);

        int w, h;
        SDL_GetWindowSize(win, &w, &h);
        glViewport(0, 0, w, (h > 0 ? h : 1));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        load_perspective(60.0f, (float)w / (float)(h > 0 ? h : 1), 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(-yaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-cam_x, -cam_y, -cam_z);

        glEnable(GL_TEXTURE_2D);

        glColor3f(1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, 1);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void *)0);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, 100 * 100 * 36);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        int chunk_x = cam_x / CHUNK_SIZE;
        int chunk_z = cam_z / CHUNK_SIZE;
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
                if (chunk * ch = world_table[chj][chi])
                {
                    int base_x = (chi)*CHUNK_SIZE;
                    int base_z = (chj)*CHUNK_SIZE;

                    for (int i = 0; i < CHUNK_SIZE; i++)
                    {
                        for (int j = 0; j < CHUNK_SIZE; j++)
                        {
                            BaseElement * be = get_base_element(ch->table[j][i].tile);
                            if (abs(chi - chunk_x) < 4 && abs(chj - chunk_z) < 4)
                                place_plane(base_x + i, 0, base_z + j, ch->table[j][i].tile % 15 + 1, be->color.r, be->color.g, be->color.b);
                            else
                                place_plane(base_x + i, 0, base_z + j, 0, be->color.r - 100, be->color.g - 100, be->color.b - 100);
                        }
                    }
                }
            }
        }

        glDisable(GL_TEXTURE_2D);
        SDL_GL_SwapWindow(win);
    }

    disconnect();
    return 0;
}
