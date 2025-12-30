use std::cell::RefCell;
use std::collections::HashMap;
use std::hash::Hash;
use std::rc::Rc;

use crate::core;
use crate::core::InventoryElement;
use crate::SEED;
use rand::prelude::*;

const REGIONS_NUM: u32 = 1000;

thread_local! {
static WORLD: RefCell<World> = panic!("world not created yet");
}

pub fn get_world_data() -> Vec<u8> {
    WORLD
        .with_borrow(|world| bincode::serialize(world))
        .unwrap()
}

#[allow(dead_code)]
pub fn show_world() {
    WORLD.with_borrow(|world| {
        println!("{:?}", world);
        /*for t in &world.terrains {
            println!("{:#?}", t.base._base);
        }

        for p in &world.plants {
            println!("{:#?}", p.get_base());
        }
        for a in &world.animals {
            println!("{:#?}", a.get_base());
        }*/
    });
}

// #[no_mangle]
// pub extern "C" fn load_chunk(map_x: i32, map_y: i32) {
//     WORLD.with_borrow(|world| {
//         let region = world
//             .regions
//             .iter()
//             .min_by_key(|r| {
//                 r.coords.distance_squared(&Coords {
//                     x: map_x as i32,
//                     y: map_y as i32,
//                 })
//             })
//             .unwrap();
//         unsafe {
//             println!("SERV: load_chunk map_x={} map_y={}", map_x, map_y);
//             //            println!("SERV: load_chunk map_x={} map_y={}  {:#?}", map_x, map_y, region);
//             let mut chunk = Box::new(core::chunk::new(map_x, map_y));
//             for y in 0..core::CHUNK_SIZE as usize {
//                 for x in 0..core::CHUNK_SIZE as usize {
//                     chunk.table[y][x] = core::tile {
//                         tile: region.terrain_type.get_id() as i32,
//                     };
//                     //DEBUG stuff
//                     //chunk.table[y][x] = core::tile { tile: ((x + y)  % 10) as i32};
//                 }
//             }
//             //println!("{:?}", region.rocks_types);
//             for (rock, num) in region.rocks_types.iter() {
//                 chunk.add_object1(core::create_element(
//                     rock.get_base() as *const core::BaseElement as *mut core::BaseElement
//                 ) as *mut core::InventoryElement);
//             }

//             chunk.add_object1(core::create_npc() as *mut core::InventoryElement);

//             for (plant, num) in region.active_plants.iter() {
//                 chunk.add_object1(core::create_plant(
//                     plant.get_base() as *const core::BasePlant as *mut core::BasePlant
//                 ) as *mut core::InventoryElement);
//                 chunk.add_object1(
//                     core::create_scroll(plant.get_base() as *const core::BasePlant
//                         as *mut core::BasePlant
//                         as *mut core::Base) as *mut core::InventoryElement,
//                 );
//             }
//             for (animal, num) in region.active_animals.iter() {
//                 chunk.add_object1(core::create_animal(
//                     animal.get_base() as *const core::BaseAnimal as *mut core::BaseAnimal
//                 ) as *mut core::InventoryElement);
//             }
//             core::world_table[map_y as usize][map_x as usize] = Box::into_raw(chunk);
//         }
//     });
// }

#[no_mangle]
pub extern "C" fn load_chunk(map_x: i32, map_y: i32) {
    WORLD.with_borrow(|world| {
        let region = world
            .regions
            .iter()
            .min_by_key(|r| {
                r.coords.distance_squared(&Coords {
                    x: map_x as i32,
                    y: map_y as i32,
                })
            })
            .unwrap();
        unsafe {
            println!("SERV: load_chunk map_x={} map_y={}", map_x, map_y);
            //            println!("SERV: load_chunk map_x={} map_y={}  {:#?}", map_x, map_y, region);
            let mut chunk = Box::new(core::chunk::new(map_x, map_y));
            for y in 0..core::CHUNK_SIZE as usize {
                for x in 0..core::CHUNK_SIZE as usize {
                    chunk.table[y][x] = core::tile {
                        tile: region.terrain_type.get_id() as i32,
                    };
                    //DEBUG stuff
                    //chunk.table[y][x] = core::tile { tile: ((x + y)  % 10) as i32};
                }
            }
            //println!("{:?}", region.rocks_types);
            for (rock, num) in region.rocks_types.iter() {
                // TODO remove +1 for each object
                let prob = num * 3.0 + 1.0;
                //                println!("element {prob} {:?}", rock);
                do_times(prob, || {
                    chunk.add_object1(core::create_element(rock.get_base()
                        as *const core::BaseElement
                        as *mut core::BaseElement)
                        as *mut core::InventoryElement);
                });
                let prob_scroll = 2.0;
                do_times(prob, || {
                    chunk.add_object1(core::create_scroll(rock.get_base()
                        as *const core::BaseElement
                        as *mut core::BaseElement
                        as *mut core::Base)
                        as *mut core::InventoryElement);
                });
            }

            chunk.add_object1(core::create_npc() as *mut core::InventoryElement);

            for (plant, num) in region.active_plants.iter() {
                let prob = num / region.size as f32 + 1.0;
                //println!("plant {prob}");
                do_times(prob, || {
                    chunk.add_object1(core::create_plant(
                        plant.get_base() as *const core::BasePlant as *mut core::BasePlant
                    ) as *mut core::InventoryElement);
                });
                let prob_scroll = 2.0;
                do_times(prob_scroll, || {
                    chunk.add_object1(core::create_scroll(
                        plant.get_base() as *const core::BasePlant as *mut core::BasePlant
                            as *mut core::Base,
                    ) as *mut core::InventoryElement);
                });
            }
            for (animal, num) in region.active_animals.iter() {
                let prob = num / region.size as f32 + 1.0;
                //println!("animal {prob}");
                do_times(prob, || {
                    chunk.add_object1(core::create_animal(animal.get_base()
                        as *const core::BaseAnimal
                        as *mut core::BaseAnimal)
                        as *mut core::InventoryElement);
                });
                let prob_scroll = 2.0;
                do_times(prob, || {
                    chunk.add_object1(core::create_scroll(animal.get_base()
                        as *const core::BaseAnimal
                        as *mut core::BaseAnimal
                        as *mut core::Base)
                        as *mut core::InventoryElement);
                });
            }
            core::world_table[map_y as usize][map_x as usize] = Box::into_raw(chunk);
        }
    });
}

fn do_times<F>(prob: f32, mut f: F)
where
    F: FnMut() -> (),
{
    if prob < 1.0 {
        if rand::random_bool(prob as f64) {
            f();
        }
    } else {
        for _ in 0..(prob * 2.0) as u32 {
            if rand::random_bool(0.5) {
                f();
            }
        }
    }
}

#[derive(Debug, serde::Serialize)]
struct World {
    terrains: Vec<Rc<TerrainType>>,
    plants: Vec<Rc<PlantType>>,
    animals: Vec<Rc<AnimalType>>,
    #[serde(skip)]
    regions: Vec<Region>,
}

impl World {
    fn new() -> World {
        let terrains = create_terrains();
        //        println!("{:#?}", terrains);
        let plants = create_plants(&terrains);
        let animals = create_animals(&plants);
        let regions = create_regions(&terrains, &plants, &animals);
        World {
            terrains,
            plants,
            animals,
            regions,
        }
    }
}

pub fn generate() {
    unsafe {
        SEED = core::time(std::ptr::null_mut()) as i64;
        println!("{}", SEED);
        core::srand(SEED as u32);
        println!("{}", core::rand());
        // core::init_elements();
        //core::generator();
    }
    let mut world = World::new();

    /*println!("{:#?}", world.plants);
    println!("{:#?}", world.animals);
    println!("{:#?}", world.regions[0]);
    */
    simulate(&mut world.regions, &mut world.plants, &mut world.animals);
    //println!("{:#?}", world.regions[0]);
    for _ in 0..10 {
        simulate(&mut world.regions, &mut world.plants, &mut world.animals);
        // for r in regions.iter() {
        //     print!(
        //         "[{}] {:7.2} {:7.2} ",
        //         r.terrain_type.id,
        //         r.active_plants.iter().map(|(_, num)| num).sum::<f32>(),
        //         r.active_animals.iter().map(|(_, num)| num).sum::<f32>(),
        //     );
        // }
        // for animal in regions[0].active_animals.iter() {
        //     print!("a{:02} {:7.2} ", animal.0.id, animal.1)
        // }
        // for plant in regions[0].active_plants.iter() {
        //     print!("p{:02} {:7.2} ", plant.0.id, plant.1)
        // }
        // println!("");
    }
    //    simulate(&mut regions);
    // println!("{:#?}", world.regions);

    WORLD.set(world);
    for y in 127..=129 {
        for x in 127..=129 {
            load_chunk(x, y);
        }
    }
    //load_chunk(128,128);
}

fn simulate(
    regions: &mut Vec<Region>,
    plants: &mut Vec<Rc<PlantType>>,
    animals: &mut Vec<Rc<AnimalType>>,
) {
    for r in regions {
        for (plant, num) in r.active_plants.iter_mut() {
            *num += *num * plant.growth_speed as f32 * plant.possible_ground[&*r.terrain_type];
        }

        //let occupied_space: f32 = (r.size as i32 - r.free_space()) as f32;
        let occupied_space = r.occupied_space() as f32;
        let sun_amount = (r.size * core::CHUNK_SIZE * core::CHUNK_SIZE) as f32;
        let mut plants_to_remove = vec![];
        if occupied_space > sun_amount {
            for (plant, num) in r.active_plants.iter_mut() {
                *num = *num / (occupied_space / sun_amount);
                if *num < 0.1 {
                    plants_to_remove.push(Rc::clone(plant));
                }
            }
        }
        for p in plants_to_remove {
            r.active_plants.remove(&p);
        }
        //        r.active_plants.iter()
        // old plants die
        //    lifespan
        // TODO some get eaten
        //    animals that live here eat
        // new plants grow depending on:
        //    num * ground_modifier * growth_speed_modifier
        //    capped at space avaible
        let mut changes = vec![];
        for (animal, num) in r.active_animals.iter() {
            let mut found_food = 0.0;
            for (plant, multiplier) in animal.possible_food_plant.iter() {
                if r.active_plants.contains_key(plant) {
                    let found = if animal.speed * num > r.size as f32 {
                        //panic!("too many animals {} {} {:?}", animal.speed, num, r)
                        r.active_plants[plant]
                    } else {
                        r.active_plants[plant] / r.size as f32 * animal.speed * num
                    };
                    *r.active_plants.get_mut(plant).unwrap() -= found / 2.0;
                    if r.active_plants[plant] < 0.1 {
                        r.active_plants.remove(plant);
                    }
                    found_food += found * multiplier;
                }
            }
            for (food, multiplier) in animal.possible_food_animal.iter() {
                if r.active_animals.contains_key(food) {
                    let found = if animal.speed * num > r.size as f32 {
                        // panic!("too many foods")
                        r.active_animals[food]
                    } else {
                        r.active_animals[food] / r.size as f32 * animal.speed * num
                    };
                    changes.push((Rc::clone(food), -found / 2.0));
                    found_food += found * multiplier;
                }
            }
            if found_food / (animal.required_food * num) > 1.2 {
                changes.push((Rc::clone(animal), num * 0.2));
            } else {
                changes.push((
                    Rc::clone(animal),
                    num * (found_food / (animal.required_food * num)) - num,
                ));
            }
        }
        for (animal, num) in changes {
            if r.active_animals.contains_key(&animal) {
                if r.active_animals[&animal] + num < 0.01 {
                    r.active_animals.remove(&animal);
                } else {
                    *r.active_animals.get_mut(&animal).unwrap() += num;
                }
            }
        }

        if rand::random_bool(0.05) {
            if r.active_plants.len() > 0 {
                let (best_plant, _) = r
                    .active_plants
                    .iter()
                    .max_by(|(_plant, num), (_plant2, num2)| num.partial_cmp(num2).unwrap())
                    .unwrap();
                if let Some(new_animal) = animals
                    .iter()
                    .filter(|animal| animal.can_eat_plant(best_plant))
                    .filter(|animal| !r.active_animals.contains_key(&Rc::clone(animal)))
                    .choose(&mut rand::rng())
                {
                    r.active_animals.insert(Rc::clone(new_animal), 5.0);
                } else {
                    let new_animal = Rc::new(AnimalType::new_eating(
                        animals.len() as u32,
                        best_plant,
                        plants,
                        animals,
                    ));
                    r.active_animals.insert(Rc::clone(&new_animal), 5.0);
                    animals.push(new_animal);

                    println!("new animal eating {}", best_plant.id);
                }
                // let new_animal = Rc::new(AnimalType::new(animals.len() as u32, plants, animals));
                // r.active_animals.insert(Rc::clone(&new_animal), 5.0);
                // animals.push(new_animal);
            }
        }

        /*for (_, num) in r.active_plants.iter() {
            if *num == std::f32::NAN || *num == f32::INFINITY {
                println!("{:#?}", r);
            }
        }

        for (_, num) in r.active_animals.iter() {
            if *num == std::f32::NAN || *num == f32::INFINITY {
                println!("{:#?}", r);
            }
        }*/

        if rand::random_bool(0.01) {
            if let Some(new_plant) = plants
                .iter()
                .filter(|plant| plant.possible_ground.contains_key(&r.terrain_type))
                .choose(&mut rand::rng())
            {
                r.active_plants.insert(
                    Rc::clone(new_plant),
                    r.free_space().min((r.total_size() / 10) as i32) as f32,
                );
            } else {
                println!("gowno {:#?}", r);
            }
            //let new_plant = Rc::clone()
        }

        if rand::random_bool(0.001) {
            let new_plant = Rc::new(PlantType::new(
                plants.len() as u32,
                &vec![Rc::clone(&r.terrain_type)],
            ));
            plants.push(Rc::clone(&new_plant));
            r.active_plants.insert(
                Rc::clone(&new_plant),
                r.free_space().min((r.total_size() / 10) as i32) as f32,
            );
        }
    }
}

fn create_terrains() -> Vec<Rc<TerrainType>> {
    let num = rand::random_range(10..20);
    //let num = rand::random_range(3..4);
    let mut terrains = Vec::<Rc<TerrainType>>::with_capacity(num);
    for i in 0..num {
        terrains.push(Rc::new(TerrainType::new(i as u32)));
    }
    terrains
}

fn create_regions(
    terrains: &Vec<Rc<TerrainType>>,
    plants: &Vec<Rc<PlantType>>,
    animals: &Vec<Rc<AnimalType>>,
) -> Vec<Region> {
    let mut rng = rand::rng();
    let mut regions = vec![];
    let mut centers = [Coords::new(0, 0); REGIONS_NUM as usize];
    for i in 0..REGIONS_NUM as usize {
        centers[i] = Coords::new(
            rng.random_range(0..core::WORLD_SIZE) as i32,
            rng.random_range(0..core::WORLD_SIZE) as i32,
        );
    }
    let mut sizes = [0u32; REGIONS_NUM as usize];
    for y in 0..core::WORLD_SIZE {
        for x in 0..core::WORLD_SIZE {
            let (center, _) = centers
                .iter_mut()
                .enumerate()
                .min_by_key(|(_, center)| center.distance_squared(&Coords::new(x as i32, y as i32)))
                .unwrap();
            sizes[center] += 1;
        }
    }

    for i in 0..REGIONS_NUM as usize {
        let r = Region::new(
            Rc::clone(terrains.choose(&mut rand::rng()).unwrap()),
            terrains,
            plants,
            animals,
            centers[i].x as u32,
            centers[i].y as u32,
            sizes[i],
        );
        regions.push(r);
    }
    regions
}

fn create_plants(terrains: &Vec<Rc<TerrainType>>) -> Vec<Rc<PlantType>> {
    let n = rand::random_range(10..20);
    let mut plants = Vec::<Rc<PlantType>>::with_capacity(n);
    for i in 0..n {
        plants.push(Rc::new(PlantType::new(i as u32, terrains)));
    }

    plants
}

fn create_animals(plants: &Vec<Rc<PlantType>>) -> Vec<Rc<AnimalType>> {
    let n = rand::random_range(20..30);
    let mut animals = Vec::<Rc<AnimalType>>::with_capacity(n);
    for i in 0..n {
        animals.push(Rc::new(AnimalType::new(i as u32, plants, &animals)));
    }

    animals
}

#[derive(Debug, Clone, Copy, serde::Serialize, serde::Deserialize)]
struct Coords {
    pub x: i32,
    pub y: i32,
}

impl Coords {
    fn new(x: i32, y: i32) -> Coords {
        Coords { x, y }
    }
    fn distance_squared(&self, other: &Coords) -> i32 {
        (self.x - other.x) * (self.x - other.x) + (self.y - other.y) * (self.y - other.y)
    }
}
/*impl serde::Serialize for core::BaseElement {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut s = serializer.serialize_struct("BaseElement", 3)?;
        s.serialize_field("density", unsafe { &(*self.density).value })?;
        s.end()
    }
}*/

// impl<'de> serde::Deserialize<'de> for core::BaseElement {
//     fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
//     where
//         D: serde::Deserializer<'de>,
//     {
//         todo!()
//     }
// }

#[derive(serde::Serialize)]
struct Region {
    pub terrain_type: Rc<TerrainType>,
    //pub liquid_type: u32,
    pub rocks_types: HashMap<Rc<TerrainType>, f32>,
    pub size: u32,
    pub coords: Coords,
    pub active_plants: HashMap<Rc<PlantType>, f32>,
    pub active_animals: HashMap<Rc<AnimalType>, f32>,
}

impl Region {
    pub fn new(
        terrain_type: Rc<TerrainType>,
        rocks: &Vec<Rc<TerrainType>>,
        plants: &Vec<Rc<PlantType>>,
        animals: &Vec<Rc<AnimalType>>,
        x: u32,
        y: u32,
        size: u32,
    ) -> Region {
        let n = rand::random_range(5..10);
        let mut active_plants = HashMap::with_capacity(n);
        let choices = plants.choose_multiple(&mut rand::rng(), n);
        for plant in choices {
            if plant.possible_ground.keys().any(|t| *t == terrain_type) {
                active_plants.insert(Rc::clone(plant), 500.0);
            }
        }
        let n = rand::random_range(15..20);
        let mut active_animals = HashMap::with_capacity(n);
        let choices = animals.choose_multiple(&mut rand::rng(), n);
        for animal in choices {
            if animal
                .possible_food_plant
                .iter()
                .any(|(food, _)| active_plants.contains_key(food))
                || animal
                    .possible_food_animal
                    .iter()
                    .any(|(food, _)| active_animals.contains_key(food))
            {
                active_animals.insert(Rc::clone(animal), 5.0);
            }
        }
        let n = rand::random_range(3..10);
        let mut rocks_types = HashMap::with_capacity(n);
        for r in rocks.choose_multiple(&mut rand::rng(), n) {
            rocks_types.insert(Rc::clone(r), rand::random_range(0.1..1.0));
        }
        Region {
            terrain_type,
            //liquid_type: 0,
            rocks_types,
            size,
            coords: Coords::new(x as i32, y as i32),
            active_plants,
            active_animals,
        }
    }
    pub fn free_space(&self) -> i32 {
        let free = (self.size * core::CHUNK_SIZE * core::CHUNK_SIZE) as i32 - self.occupied_space();
        free as i32
    }
    pub fn occupied_space(&self) -> i32 {
        let occupied = self
            .active_plants
            .iter()
            .map(|(plant, num)| plant.size as f32 * num)
            .sum::<f32>() as i32;
        occupied
    }
    pub fn total_size(&self) -> u32 {
        self.size * core::CHUNK_SIZE * core::CHUNK_SIZE
    }
}

impl std::fmt::Debug for Region {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_fmt(format_args!(
            "Region: \n    size: {:?}\n    terrain: {:?}\n",
            self.size,
            self.terrain_type.get_id()
        ))?;
        for (plant, num) in self.active_plants.iter() {
            f.write_fmt(format_args!("Plant {}: {}\n", plant.id, num))?;
        }
        for (animal, num) in self.active_animals.iter() {
            f.write_fmt(format_args!("Animal {} {}\n", animal.id, num))?;
        }
        f.write_str("\n")
    }
}

#[repr(C)]
#[derive(Debug, serde::Serialize)]
pub struct TerrainType {
    #[serde(skip)]
    id: u32,
    base: core::BaseElement,
}

impl TerrainType {
    pub fn get_id(&self) -> u32 {
        self.id
    }

    pub fn get_base(&self) -> &core::BaseElement {
        &self.base
    }

    pub fn new(id: u32) -> TerrainType {
        let f = match rand::random_range(0..10) {
            0..=7 => core::Form_Form_solid,
            8..=9 => core::Form_Form_liquid,
            10 => core::Form_Form_gas,
            _ => unreachable!(),
        };
        TerrainType {
            id,
            base: unsafe { core::BaseElement::new(f, id as i32) },
        }
    }
}

impl PartialEq for TerrainType {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for TerrainType {}

impl Hash for TerrainType {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}
#[derive(Debug, serde::Serialize)]
pub struct PlantType {
    #[serde(skip)]
    pub id: u32,
    #[serde(skip)]
    pub possible_ground: HashMap<Rc<TerrainType>, f32>,
    #[serde(skip)]
    pub size: u32,
    //pub sun_required: u32,
    #[serde(skip)]
    pub growth_speed: f32,
    // TODO
    // overcrowd immunity
    base: core::BasePlant,
}

impl PlantType {
    pub fn new(id: u32, terrains: &Vec<Rc<TerrainType>>) -> PlantType {
        let n = rand::random_range(terrains.len() / 2..terrains.len() + 1);
        let mut possible_ground = HashMap::new();
        possible_ground.insert(Rc::clone(terrains.choose(&mut rand::rng()).unwrap()), 1.0);
        for _ in 1..n {
            let choice = terrains.choose(&mut rand::rng()).unwrap();
            if !possible_ground.contains_key(&**choice) {
                possible_ground.insert(Rc::clone(choice), rand::random_range(0.1..1.2));
            }
        }
        PlantType {
            id,
            possible_ground,
            size: rand::random_range(1..3),
            growth_speed: rand::random_range(0.1..0.4),
            base: unsafe { core::BasePlant::new(id as i32) },
        }
    }

    #[allow(dead_code)]
    pub fn get_base(&self) -> &core::BasePlant {
        &self.base
    }
}

impl PartialEq for PlantType {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for PlantType {}

impl std::hash::Hash for PlantType {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}

#[derive(serde::Serialize)]
pub struct AnimalType {
    #[serde(skip)]
    pub id: u32,
    #[serde(skip)]
    pub possible_food_plant: Vec<(Rc<PlantType>, f32)>,
    #[serde(skip)]
    pub possible_food_animal: Vec<(Rc<AnimalType>, f32)>,
    #[serde(skip)]
    pub required_food: f32,
    #[serde(skip)]
    pub speed: f32,
    base: core::BaseAnimal,
}

impl AnimalType {
    pub fn new(id: u32, plants: &Vec<Rc<PlantType>>, animals: &Vec<Rc<AnimalType>>) -> AnimalType {
        let food_num = rand::random_range(1..5);
        let mut possible_food_plant: Vec<(Rc<PlantType>, f32)> = Vec::new();
        let mut possible_food_animal: Vec<(Rc<AnimalType>, f32)> = Vec::new();
        if animals.len() > 0 {
            let animal_food = rand::random_range(0..animals.len().min(food_num));
            for food in plants.choose_multiple(&mut rand::rng(), food_num - animal_food) {
                possible_food_plant.push((Rc::clone(food), rand::random_range(0.7..1.5)));
            }
            for food in animals.choose_multiple(&mut rand::rng(), animal_food) {
                possible_food_animal.push((Rc::clone(food), rand::random_range(0.7..1.5)));
            }
        } else {
            for food in plants.choose_multiple(&mut rand::rng(), food_num) {
                possible_food_plant.push((Rc::clone(food), rand::random_range(0.7..1.5)));
            }
        }
        let speed = rand::random_range(16.0..64.0);
        let required_food = speed / 64.0 * speed / 64.0;
        AnimalType {
            id,
            possible_food_plant,
            possible_food_animal,
            required_food,
            speed,
            base: unsafe { core::BaseAnimal::new(id as i32) },
        }
    }
    pub fn new_eating(
        id: u32,
        plant: &Rc<PlantType>,
        plants: &Vec<Rc<PlantType>>,
        animals: &Vec<Rc<AnimalType>>,
    ) -> AnimalType {
        let mut animal = AnimalType::new(id, plants, animals);
        if !animal.possible_food_plant.iter().any(|(p, _)| p == plant) {
            animal
                .possible_food_plant
                .push((Rc::clone(plant), rand::random_range(0.7..1.5)));
        }
        animal
    }
    #[allow(dead_code)]
    pub fn get_base(&self) -> &core::BaseAnimal {
        &self.base
    }
    pub fn can_eat_plant(&self, plant: &Rc<PlantType>) -> bool {
        for (p, _) in self.possible_food_plant.iter() {
            if p == plant {
                return true;
            }
        }
        false
    }
}

impl std::fmt::Debug for AnimalType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AnimalType")
            .field("id", &self.id)
            .field("speed", &self.speed)
            .field("required food", &self.required_food);
        f.write_fmt(format_args!("Food:\n"))?;
        for (plant, multiplier) in self.possible_food_plant.iter() {
            f.write_fmt(format_args!("    Plant {}: {}\n", plant.id, multiplier))?;
        }
        for (food, multiplier) in self.possible_food_animal.iter() {
            f.write_fmt(format_args!("    Animal {}: {}\n", food.id, multiplier))?;
        }
        f.write_str("}")
    }
}

impl PartialEq for AnimalType {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for AnimalType {}

impl std::hash::Hash for AnimalType {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}
