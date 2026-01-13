#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <climits>
#include "../../core/world_params.h"
int CONSOLE_LOG(const char * fmt, ...);

#define REGIONS_NUM 16

int random_range(int min, int max);

class Coords
{
  public:
    int x;
    int y;
    Coords(int x, int y) : x(x), y(y)
    {
    }
    Coords()
    {
        x = rand() % WORLD_SIZE;
        y = rand() % WORLD_SIZE;
    }
    int distance_squared(Coords * other)
    {
        int dx = x - other->x;
        int dy = y - other->y;
        return dx * dx + dy * dy;
    }
};

class TerrainType
{
  public:
    int form;
    int id;
    TerrainType(int id) : id(id)
    {
        form = random_range(1, 4);
        //form = 1;
    }
    bool eq(TerrainType * other)
    {
        return id == other->id;
    }
    void show()
    {
        CONSOLE_LOG(" TerrainType: id=%d form=%d\n", id, form);
    }
};

class PlantType
{
  public:
    int id; // id of BasePlant
    TerrainType ** possible_ground;
    int grounds_count;
    PlantType(int id);
    void show()
    {
        CONSOLE_LOG(" PlanType: id=%d grounds=%d: ", id, grounds_count);
        for (int i=0; i< grounds_count; i++)
        {
            CONSOLE_LOG("%d ", possible_ground[i]->id);
        }
        CONSOLE_LOG("\n");
    }
    bool check_ground(int g)
    {
        for (int i = 0; i < grounds_count; i++)
        {
            if (possible_ground[i]->id == g)
                return true;
        }
        return false;
    }
};

class AnimalType
{
  public:
    int id; // id of BaseAnimal
    TerrainType ** possible_ground;
    int grounds_count;
    AnimalType(int id);
    void show()
    {
        CONSOLE_LOG(" AnimalType: id=%d grounds=%d: ", id, grounds_count);
        for (int i=0; i< grounds_count; i++)
        {
            CONSOLE_LOG("%d ", possible_ground[i]->id);
        }
        CONSOLE_LOG("\n");
    }
    bool check_ground(int g)
    {
        for (int i = 0; i < grounds_count; i++)
        {
            if (possible_ground[i]->id == g)
                return true;
        }
        return false;
    }
};


extern int terrains_count;
extern TerrainType ** terrains;
extern int all_plants_count;
extern PlantType ** all_plants;
extern int all_animals_count;
extern AnimalType ** all_animals;
extern int all_base_elements_count;

class RockEntry
{
  public:
    TerrainType * terrain;
    float value;
    RockEntry(TerrainType * t, float v) : terrain(t), value(v)
    {
    }
    void show()
    {
        terrain->show();
        CONSOLE_LOG("      RockEntry: value=%f\n", value);
    }
};

class PlantEntry
{
  public:
    PlantType * plant;
    float value;
    PlantEntry(PlantType * p, float v) : plant(p), value(v)
    {
    }
    void show()
    {
        plant->show();
        CONSOLE_LOG("      PlantEntry: value=%f\n", value);
    }
};


class AnimalEntry
{
  public:
    AnimalType * animal;
    float value;
    AnimalEntry(AnimalType * p, float v) : animal(p), value(v)
    {
    }
    void show()
    {
        animal->show();
        CONSOLE_LOG("      AnimalEntry: value=%f\n", value);
    }
};

class Region
{
  public:
    TerrainType * terrain_type;
    RockEntry ** rocks_types; // elements in this region
    int rocks_count; //number of elements in this region
    PlantEntry ** plants_types;
    int plants_count;
    AnimalEntry ** animals_types;
    int animals_count;

    unsigned int size;
    Coords coords;
    Region(TerrainType * terrain_type, int x, int y, unsigned int size);
    ~Region()
    {
        delete rocks_types;
    }
    unsigned int total_size()
    {
        return size * CHUNK_SIZE * CHUNK_SIZE;
    }
    void show()
    {
        CONSOLE_LOG("Region terrain(id=%d form=%d) size: %u\n", terrain_type->id, terrain_type->form, total_size());
        CONSOLE_LOG("%d rocks types in region:\n", rocks_count);
        for (int i = 0; i < rocks_count; i++)
        {
            CONSOLE_LOG("%d: ", i);
            rocks_types[i]->show();
        }
        CONSOLE_LOG("%d plants:\n", plants_count);
        for (int i = 0; i < plants_count; i++)
        {
            CONSOLE_LOG("%d: ", i);
            plants_types[i]->show();
        }
        CONSOLE_LOG("%d animals:\n", animals_count);
        for (int i = 0; i < animals_count; i++)
        {
            CONSOLE_LOG("%d: ", i);
            animals_types[i]->show();
        }
    }
};

extern Region ** regions;
void create_regions();

Region * find_region(int x, int y);
void show_terrains();
void show_plants();
void show_animals();

