#pragma once
#include "../core/world_params.h"
#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <cmath>

typedef struct OGL_Camera
{
    float x, y, z;
    float pitch, yaw;
    // ----- yaw
    //
    // |
    // | pitch
    // |
    float fov;
    float znear, zfar;

    OGL_Camera() : x(128 * CHUNK_SIZE + 8.5), y(2), z(128 * CHUNK_SIZE + 8.5), pitch(0), yaw(0), fov(90), znear(0.1), zfar(1000)
    {
    }

    static void load_perspective(float fovy_deg, float aspect, float znear, float zfar)
    {
        float fovy = fovy_deg * (float)M_PI / 180.0f;
        float f = 1.0f / tanf(fovy * 0.5f);
        float nf = 1.0f / (znear - zfar);

        // Column-major order for glMultMatrixf (OpenGL expects column-major)
        float m[16] = {f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (zfar + znear) * nf, -1, 0, 0, (2.0f * zfar * znear) * nf, 0};

        glMultMatrixf(m);
    }

    void begin_camera(SDL_Window * window)
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, (h > 0 ? h : 1));

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        load_perspective(90.0f, (float)w / (float)(h > 0 ? h : 1), 0.1f, 2000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(-yaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-x, -y, -z);
    }
} OGL_Camera;

typedef struct OGL_Vertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    GLuint tex_id;
} OGL_Vertex;

typedef struct OGL_Plane
{
    float x, y, z;
    GLuint texture;
    int r, g, b;
    OGL_Vertex vertices[6];
    OGL_Plane(float x, float y, float z, GLuint texture, int r, int g, int b) : x(x), y(y), z(z), texture(texture), r(r), g(g), b(b)
    {
        int idx = 0;
        vertices[idx++] = (OGL_Vertex){x - 0.5f, y + 0.5f, z + 0.5f, 0, 1, 0, 0, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + 0.5f, y + 0.5f, z + 0.5f, 0, 1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + 0.5f, y + 0.5f, z - 0.5f, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + 0.5f, y + 0.5f, z - 0.5f, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - 0.5f, y + 0.5f, z - 0.5f, 0, 1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - 0.5f, y + 0.5f, z + 0.5f, 0, 1, 0, 0, 0, texture};
    };
    void render()
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f((double)r / 255, (double)g / 255, (double)b / 255);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(OGL_Vertex), &vertices[0].x);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(OGL_Vertex), &vertices[0].u);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
} OGL_Plane;

typedef struct OGL_Chunk
{
    OGL_Plane * planes[CHUNK_SIZE * CHUNK_SIZE];
    void render()
    {
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
        {
            planes[i]->render();
        }
    }
    OGL_Chunk()
    {
    }
} OGL_Chunk;
