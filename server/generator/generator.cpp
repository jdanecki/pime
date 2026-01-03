#include "generator.h"

int terrains_count;
TerrainType ** terrains;

int all_plants_count;
PlantType ** all_plants;

int all_animals_count;
AnimalType ** all_animals;

int all_base_elements_count;

int random_range(int min, int max)
{
    return rand() % (max - min) + min;
}

float random_float_range(float min, float max)
{
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void choose_multiple(int total, int n, int * out_indices)
{
    int * indices = new int[total];
    for (int i = 0; i < total; i++)
    {
        indices[i] = i;
    }

    for (int i = total - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
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
    terrains_count = TILE_TEXTURES; //random_range(10, TILE_TEXTURES);
    terrains = new TerrainType *[terrains_count];
    for (int i = 0; i < terrains_count; i++)
    {
        terrains[i] = new TerrainType(i);
    }
    CONSOLE_LOG("terrains count=%d\n", terrains_count);    

    all_plants_count = BASE_PLANTS; //random_range(10, BASE_PLANTS);
    all_plants = new PlantType *[all_plants_count];
    for (int i = 0; i < all_plants_count; i++)
    {
        all_plants[i] = new PlantType(i);
    }
    CONSOLE_LOG("plants count=%d\n", all_plants_count);

    all_animals_count = BASE_ANIMALS;//random_range(10, 20);
    all_animals = new AnimalType *[all_animals_count];
    for (int i = 0; i < all_animals_count; i++)
    {
        all_animals[i] = new AnimalType(i);
    }
    CONSOLE_LOG("animals count=%d\n", all_animals_count);

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
