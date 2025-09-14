#include "generator.h"

int terrains_count;
TerrainType ** terrains;

int random_range(int min, int max)
{
    return rand() % (max - min) + min;
}

float random_float_range(float min, float max)
{
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}

void choose_multiple(int total, int n, int* out_indices)
{
    int* indices = new int[total];
    for (int i = 0; i < total; i++) {
        indices[i] = i;
    }

    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    for (int i = 0; i < n; i++) {
        out_indices[i] = indices[i];
    }

    delete [] indices;
}

Region::Region(TerrainType* terrain_type, TerrainType** rocks, int rocks_len, int x, int y, unsigned int size):
        terrain_type(terrain_type), rocks_count(rocks_len), size(size), coords(Coords(x,y))
{
    int n = random_range(3, 10);
    if (n > rocks_len) n = rocks_len;

    rocks_types = new RockEntry*[n];
    int* chosen_indices = new int[n];
    choose_multiple(rocks_len, n, chosen_indices);

    for (int i = 0; i < n; i++) {
        int idx = chosen_indices[i];
        rocks_types[i]= new RockEntry(rocks[idx], random_float_range(0.1f, 1.0f));
    }
    rocks_count = n;
    delete [] chosen_indices;
}

void create_regions(
    //PlantType** plants, size_t plant_count,
    //AnimalType** animals, size_t animal_count
    )
{
    terrains_count = random_range(10, 20);
    terrains = new TerrainType*[terrains_count];
    for (int i=0; i < terrains_count; i++)
    {
        terrains[i] = new TerrainType(i);
    }
    printf("terrains count=%d\n", terrains_count);
    regions = new Region*[REGIONS_NUM];
    unsigned int sizes[REGIONS_NUM] = {0};

    Coords *centers = new Coords[REGIONS_NUM];

    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            Coords point = {x, y};
            int min_dist = INT_MAX;
            int closest_center = 0;
            for (int i = 0; i < REGIONS_NUM; i++) {
                int dist = point.distance_squared(&centers[i]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_center = i;
                }
            }
            sizes[closest_center]++;
        }
    }

    for (int i = 0; i < REGIONS_NUM; i++) {
        TerrainType* random_terrain = terrains[rand() % terrains_count];
        regions[i] = new Region(
            random_terrain,
            terrains, terrains_count,
          //  plants, plant_count,
          //  animals, animal_count,
            centers[i].x,
            centers[i].y,
            sizes[i]
            );
    }
}

Region * find_region(int x, int y)
{
    Coords point = {x, y};
    int min_dist = INT_MAX;
    int closest_region = 0;

    for (int i = 0; i < REGIONS_NUM; i++) {
        int dist = point.distance_squared(&regions[i]->coords);
        if (dist < min_dist) {
            min_dist = dist;
            closest_region = i;
        }
    }
    return regions[closest_region];
}


