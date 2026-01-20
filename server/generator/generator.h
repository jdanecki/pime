
#include <stdlib.h>
//#include <time.h>
#include <climits>

int CONSOLE_LOG(const char * fmt, ...);

#define REGIONS_NUM 16

int random_range(int min, int max);

class Coords
{
  public:
    int x;
    int y;
    Coords(int x, int y);
    Coords();
    int distance_squared(Coords * other);
};

class TerrainType
{
  public:
    int form;
    int id;
    TerrainType(int id);
    bool eq(TerrainType * other);
    void show();
};

class PlantType
{
  public:
    int id; // id of BasePlant
    TerrainType ** possible_ground;
    int grounds_count;
    PlantType(int id);
    void show();
    bool check_ground(int g);
};

class AnimalType
{
  public:
    int id; // id of BaseAnimal
    TerrainType ** possible_ground;
    int grounds_count;
    AnimalType(int id);
    void show();
    bool check_ground(int g);
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
    RockEntry(TerrainType * t, float v);
    void show();
};

class PlantEntry
{
  public:
    PlantType * plant;
    float value;
    PlantEntry(PlantType * p, float v);
    void show();
};

class AnimalEntry
{
  public:
    AnimalType * animal;
    float value;
    AnimalEntry(AnimalType * p, float v);
    void show();
};

class Region
{
  public:
    TerrainType * terrain_type;
    RockEntry ** rocks_types; // elements in this region
    int rocks_count;          // number of elements in this region
    PlantEntry ** plants_types;
    int plants_count;
    AnimalEntry ** animals_types;
    int animals_count;

    unsigned int size;
    Coords coords;
    Region(TerrainType * terrain_type, int x, int y, unsigned int size);
    ~Region();
    unsigned int total_size();
    void show();
};

extern Region ** regions;
void create_regions();

Region * find_region(int x, int y);
void show_terrains();
void show_plants();
void show_animals();
