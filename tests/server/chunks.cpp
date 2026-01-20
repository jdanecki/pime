#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct ItemLocation
{
    unsigned int map_x;
    unsigned int map_y;
    unsigned int x;
    unsigned int y;
} location;

#define CHUNK_SIZE 17
typedef void (*move_function)(int dx, int dy);

void move2(int dx, int dy)
{
    int x = location.x + dx;
    int y = location.y + dy;

    int map_dx = x / CHUNK_SIZE;
    int map_dy = y / CHUNK_SIZE;

    if (x < 0 && x % CHUNK_SIZE != 0)
        map_dx--;
    if (y < 0 && y % CHUNK_SIZE != 0)
        map_dy--;

    int new_x = x - map_dx * CHUNK_SIZE;
    int new_y = y - map_dy * CHUNK_SIZE;

    int new_map_x = location.map_x + map_dx;
    int new_map_y = location.map_y + map_dy;

    if (new_map_x != location.map_x || new_map_y != location.map_y)
    {
        // printf("load chunk: %d %d\n", new_map_x, new_map_y);
    }

    location.x = new_x;
    location.y = new_y;
    location.map_x = new_map_x;
    location.map_y = new_map_y;
    // printf("player moved to dx=%d dy=%d [%d,%d][%d,%d]\n", dx, dy, new_map_x, new_map_y, new_x, new_y);
}

void move1(int dx, int dy)
{
    ItemLocation old = location;

    int new_x = location.x + dx;
    int new_y = location.y + dy;
    int new_map_x = location.map_x;
    int new_map_y = location.map_y;

    while (new_x < 0)
    {
        new_x += CHUNK_SIZE;
        new_map_x--;
    }
    while (new_x >= CHUNK_SIZE)
    {
        new_x -= CHUNK_SIZE;
        new_map_x++;
    }

    while (new_y < 0)
    {
        new_y += CHUNK_SIZE;
        new_map_y--;
    }
    while (new_y >= CHUNK_SIZE)
    {
        new_y -= CHUNK_SIZE;
        new_map_y++;
    }

    if (new_map_x != location.map_x || new_map_y != location.map_y)
    {
        //  printf("load chunk: %d %d\n", new_map_x, new_map_y);
    }

    location.x = new_x;
    location.y = new_y;
    location.map_x = new_map_x;
    location.map_y = new_map_y;
    // printf("player moved to dx=%d dy=%d [%d,%d][%d,%d]\n", dx, dy, new_map_x, new_map_y, new_x, new_y);
}

void move_by(move_function move, int dx, int dy)
{
    move(dx, 0);
    move(0, dy);
    move(-dx, 0);
    move(0, -dy);
    if (location.map_x != 128 || location.map_y != 128 || location.x != 8 || location.y != 8)
    {
        printf("player moved to wrong location dx=%d dy=%d [%d,%d][%d,%d]\n", dx, dy, location.map_x, location.map_y, location.x, location.y);
        exit(1);
    }
}
int main(int argc, char ** argv)
{
    location.map_x = 128;
    location.map_y = 128;
    location.x = 8;
    location.y = 8;
    printf("player started at [%d,%d][%d,%d]\n", location.map_x, location.map_y, location.x, location.y);

    int max_moves = 10;
    for (int i = 0; i < max_moves; i++)
    {
        printf("move %d/%d\n", i, max_moves);
        move_by(move1, rand() % (CHUNK_SIZE * 2) - CHUNK_SIZE, rand() % (CHUNK_SIZE * 2) - CHUNK_SIZE);
        move_by(move2, rand() % (CHUNK_SIZE * 2) - CHUNK_SIZE, rand() % (CHUNK_SIZE * 2) - CHUNK_SIZE);
    }
    printf("move %d/%d\n", max_moves, max_moves);

    return 0;
}