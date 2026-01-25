
#include "generator.h"
#include "../../core/alchemist/random_functions.h"
#include "../../core/world_params.h"

int terrains_count;
TerrainType ** terrains;
Region ** regions;

int all_plants_count;
PlantType ** all_plants;

int all_animals_count;
AnimalType ** all_animals;

int all_base_elements_count;

void choose_multiple(int total, int n, int * out_indices)
{
    int * indices = new int[total];
    for (int i = 0; i < total; i++)
    {
        indices[i] = i;
    }

    for (int i = total - 1; i > 0; i--)
    {
        int j = random_range(0, i);
        int tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    for (int i = 0; i < n; i++)
    {
        out_indices[i] = indices[i];
    }

    delete[] indices;
}

Region::Region(TerrainType * terrain_type, int x, int y, unsigned int size) : terrain_type(terrain_type), size(size), coords(Coords(x, y))
{
    int n = random_range(5, terrains_count);

    rocks_types = new RockEntry *[n];
    int * chosen_indices = new int[n];
    choose_multiple(terrains_count, n, chosen_indices);

    for (int i = 0; i < n; i++)
    {
        int idx = chosen_indices[i];
        rocks_types[i] = new RockEntry(terrains[idx], random_float_range(0.5f, 1.0f));
    }
    rocks_count = n;
    delete[] chosen_indices;

    n = random_range(1, all_plants_count);

    plants_types = new PlantEntry *[n];
    chosen_indices = new int[n];
    choose_multiple(all_plants_count, n, chosen_indices);
    int p = 0;
    for (int i = 0; i < n; i++)
    {
        int idx = chosen_indices[i];
        if (all_plants[idx]->check_ground(terrain_type->id))
        {
            plants_types[p] = new PlantEntry(all_plants[idx], random_float_range(0.5f, 1.0f));
            p++;
        }
    }
    plants_count = p;
    delete[] chosen_indices;

    n = random_range(1, all_animals_count);

    animals_types = new AnimalEntry *[n];
    chosen_indices = new int[n];
    choose_multiple(all_animals_count, n, chosen_indices);
    p = 0;
    for (int i = 0; i < n; i++)
    {
        int idx = chosen_indices[i];
        if (all_animals[idx]->check_ground(terrain_type->id))
        {
            animals_types[p] = new AnimalEntry(all_animals[idx], random_float_range(0.1f, 1.0f));
            p++;
        }
    }
    animals_count = p;
    delete[] chosen_indices;
}

void create_regions()
{
    terrains_count = TILE_TEXTURES; // random_range(10, TILE_TEXTURES);
    terrains = new TerrainType *[terrains_count];
    for (int i = 0; i < terrains_count; i++)
    {
        terrains[i] = new TerrainType(i);
    }
    CONSOLE_LOG("terrains types count=%d\n", terrains_count);

    all_plants_count = BASE_PLANTS; // random_range(10, BASE_PLANTS);
    all_plants = new PlantType *[all_plants_count];
    for (int i = 0; i < all_plants_count; i++)
    {
        all_plants[i] = new PlantType(i);
    }
    CONSOLE_LOG("plants types count=%d\n", all_plants_count);

    all_animals_count = BASE_ANIMALS; // random_range(10, 20);
    all_animals = new AnimalType *[all_animals_count];
    for (int i = 0; i < all_animals_count; i++)
    {
        all_animals[i] = new AnimalType(i);
    }
    CONSOLE_LOG("animals types count=%d\n", all_animals_count);

    regions = new Region *[REGIONS_NUM];
    unsigned int sizes[REGIONS_NUM] = {0};

    Coords * centers = new Coords[REGIONS_NUM];

    for (int y = 0; y < WORLD_SIZE; y++)
    {
        for (int x = 0; x < WORLD_SIZE; x++)
        {
            Coords point = {x, y};
            int min_dist = INT_MAX;
            int closest_center = 0;
            for (int i = 0; i < REGIONS_NUM; i++)
            {
                int dist = point.distance_squared(&centers[i]);
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_center = i;
                }
            }
            sizes[closest_center]++;
        }
    }

    for (int i = 0; i < REGIONS_NUM; i++)
    {
        TerrainType * random_terrain = terrains[rand() % terrains_count];
        regions[i] = new Region(random_terrain, centers[i].x, centers[i].y, sizes[i]);
    }

    all_base_elements_count = 32;
    CONSOLE_LOG("base_elements count=%d\n", all_base_elements_count);
}

Region * find_region(int x, int y)
{
    Coords point = {x, y};
    int min_dist = INT_MAX;
    int closest_region = 0;

    for (int i = 0; i < REGIONS_NUM; i++)
    {
        int dist = point.distance_squared(&regions[i]->coords);
        if (dist < min_dist)
        {
            min_dist = dist;
            closest_region = i;
        }
    }
    return regions[closest_region];
}

PlantType::PlantType(int id) : id(id)
{
    int n = random_range(3, terrains_count);

    possible_ground = new TerrainType *[n];
    int * chosen_indices = new int[n];
    choose_multiple(terrains_count, n, chosen_indices);

    for (int i = 0; i < n; i++)
    {
        int idx = chosen_indices[i];
        possible_ground[i] = terrains[idx];
    }
    grounds_count = n;
    delete[] chosen_indices;
}

AnimalType::AnimalType(int id) : id(id)
{
    int n = random_range(3, terrains_count);

    possible_ground = new TerrainType *[n];
    int * chosen_indices = new int[n];
    choose_multiple(terrains_count, n, chosen_indices);

    for (int i = 0; i < n; i++)
    {
        int idx = chosen_indices[i];
        possible_ground[i] = terrains[idx];
    }
    grounds_count = n;
    delete[] chosen_indices;
}

void show_terrains()
{
    for (int i = 0; i < terrains_count; i++)
    {
        terrains[i]->show();
    }
}

void show_plants()
{
    for (int i = 0; i < all_plants_count; i++)
    {
        all_plants[i]->show();
    }
}

void show_animals()
{
    for (int i = 0; i < all_animals_count; i++)
    {
        all_animals[i]->show();
    }
}
Coords::Coords(int x, int y) : x(x), y(y)
{
}
Coords::Coords()
{
    x = random_range(0, WORLD_SIZE - 1);
    y = random_range(0, WORLD_SIZE - 1);
}
int Coords::distance_squared(Coords * other)
{
    int dx = x - other->x;
    int dy = y - other->y;
    return dx * dx + dy * dy;
}
TerrainType::TerrainType(int id) : id(id)
{
    form = random_range(1, 4);
    // form = 1;
}
bool TerrainType::eq(TerrainType * other)
{
    return id == other->id;
}
void TerrainType::show()
{
    CONSOLE_LOG(" TerrainType: id=%d form=%d\n", id, form);
}
void PlantType::show()
{
    CONSOLE_LOG(" PlanType: id=%d grounds=%d: ", id, grounds_count);
    for (int i = 0; i < grounds_count; i++)
    {
        CONSOLE_LOG("%d ", possible_ground[i]->id);
    }
    CONSOLE_LOG("\n");
}
bool PlantType::check_ground(int g)
{
    for (int i = 0; i < grounds_count; i++)
    {
        if (possible_ground[i]->id == g)
            return true;
    }
    return false;
}
void AnimalType::show()
{
    CONSOLE_LOG(" AnimalType: id=%d grounds=%d: ", id, grounds_count);
    for (int i = 0; i < grounds_count; i++)
    {
        CONSOLE_LOG("%d ", possible_ground[i]->id);
    }
    CONSOLE_LOG("\n");
}
bool AnimalType::check_ground(int g)
{
    for (int i = 0; i < grounds_count; i++)
    {
        if (possible_ground[i]->id == g)
            return true;
    }
    return false;
}
RockEntry::RockEntry(TerrainType * t, float v) : terrain(t), value(v)
{
}
void RockEntry::show()
{
    terrain->show();
    CONSOLE_LOG("      RockEntry: value=%f\n", value);
}
PlantEntry::PlantEntry(PlantType * p, float v) : plant(p), value(v)
{
}
void PlantEntry::show()
{
    plant->show();
    CONSOLE_LOG("      PlantEntry: value=%f\n", value);
}
AnimalEntry::AnimalEntry(AnimalType * p, float v) : animal(p), value(v)
{
}
void AnimalEntry::show()
{
    animal->show();
    CONSOLE_LOG("      AnimalEntry: value=%f\n", value);
}
Region::~Region()
{
    delete rocks_types;
}
unsigned int Region::total_size()
{
    return size * CHUNK_SIZE * CHUNK_SIZE;
}
void Region::show()
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
