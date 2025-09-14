#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <climits>

int random_range(int min, int max);

class Coords
{
  public:
    int x;
    int y;
    Coords(int x, int y): x(x), y(y) {}
    Coords()  {
        x = rand() % WORLD_SIZE;
        y = rand() % WORLD_SIZE;
    }
    int distance_squared(Coords *other)
    {
        int dx = x - other->x;
        int dy = y - other->y;
        return   dx * dx + dy * dy;
    }
};

class TerrainType {
  public:
    int form;
    int id;
    TerrainType(int id) : id(id)
    {
        form = random_range(1, 4);
    }
    bool eq(TerrainType * other)
    {
        return id == other->id;
    }
    void show() {
        printf(" TerrainType: id=%d form=%d\n", id, form);
    }
};
extern int terrains_count;
extern TerrainType ** terrains;

class RockEntry
{
  public:
    TerrainType* terrain;
    float value;
    RockEntry(TerrainType *t, float v): terrain(t), value(v) {}
    void show()
    {
        terrain->show();
        printf("      RockEntry: value=%f\n", value);
    }
};

class Region
{
  public:
    TerrainType* terrain_type;
    RockEntry** rocks_types; //elements in this region
    int rocks_count;
    unsigned int size;
    Coords coords;
    Region(TerrainType* terrain_type, TerrainType** rocks, int rocks_len, int x, int y, unsigned int size);
    ~Region()
    {
        delete rocks_types;
    }
    unsigned int total_size() {
        return size * CHUNK_SIZE * CHUNK_SIZE;
    }
    void show()
    {
        printf("Region size: %u\n", total_size());
        printf("%d rocks types in region:\n", rocks_count);
        for (int i = 0; i < rocks_count; i++)
        {
            printf("%d: ", i);
            rocks_types[i]->show();
        }
    }
};

extern Region **regions;

void create_regions(
                     //PlantType** plants, size_t plant_count,
                     //AnimalType** animals, size_t animal_count
    );
Region * find_region(int x, int y);
