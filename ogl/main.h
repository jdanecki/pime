#pragma once
#include "../core/tiles.h"
#include <GL/gl.h>
void place_cube(float tx, float ty, float tz);

typedef struct Vertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    GLuint tex_id;
} Vertex;

typedef struct OGL_Plane
{
    float x, y, z;
    GLuint texture;
    int r, g, b;
    Vertex vertices[6];
    OGL_Plane(float x, float y, float z, GLuint texture, int r, int g, int b) : x(x), y(y), z(z), texture(texture), r(r), g(g), b(b)
    {
        int idx = 0;
        vertices[idx++] = (Vertex){x - 0.5f, y + 0.5f, z + 0.5f, 0, 1, 0, 0, 0, texture};
        vertices[idx++] = (Vertex){x + 0.5f, y + 0.5f, z + 0.5f, 0, 1, 0, 1, 0, texture};
        vertices[idx++] = (Vertex){x + 0.5f, y + 0.5f, z - 0.5f, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (Vertex){x + 0.5f, y + 0.5f, z - 0.5f, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (Vertex){x - 0.5f, y + 0.5f, z - 0.5f, 0, 1, 0, 0, 1, texture};
        vertices[idx++] = (Vertex){x - 0.5f, y + 0.5f, z + 0.5f, 0, 1, 0, 0, 0, texture};
    };
    void render()
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f((double)r / 255, (double)g / 255, (double)b / 255);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].x);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].u);

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

extern chunk * world_table[WORLD_SIZE][WORLD_SIZE];
extern OGL_Chunk * ogl_tiles[WORLD_SIZE][WORLD_SIZE];
extern size_t my_id;
extern float cam_x;
extern float cam_z;
