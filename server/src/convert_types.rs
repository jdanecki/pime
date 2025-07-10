use crate::core;
use crate::types::*;

pub fn convert_to_data(el: &core::InventoryElement) -> ObjectData {
    match el.c_id {
        core::Class_id_Class_Element => {
            let element =
                unsafe { &*(el as *const core::InventoryElement as *const core::Element) };
            ObjectData::Element {
                data: ElementData {
                    base: convert_inv_el(el),
                    id: unsafe { (*element.base)._base.id },
                    sharpness: unsafe { (*element.sharpness).value },
                    smoothness: unsafe { (*element.smoothness).value },
                    mass: unsafe { (*element.mass).value },
                    length: unsafe { (*element.length).value },
                    width: unsafe { (*element.width).value },
                    height: unsafe { (*element.height).value },
                    volume: unsafe { (*element.volume).value },
                },
            }
        }
        core::Class_id_Class_Ingredient => {
            let ingredient =
                unsafe { &*(el as *const core::InventoryElement as *const core::Ingredient) };
            ObjectData::Ingredient {
                data: IngredientData {
                    base: convert_inv_el(el),
                    id: ingredient.id,
                    quality: unsafe { (*ingredient.quality).value },
                    resilience: unsafe { (*ingredient.resilience).value },
                    usage: unsafe { (*ingredient.usage).value },
                },
            }
        }
        core::Class_id_Class_Product => {
            let product =
                unsafe { &*(el as *const core::InventoryElement as *const core::Product) };
            ObjectData::Product {
                data: ProductData {
                    base: convert_inv_el(el),
                    id: product.id,
                    quality: unsafe { (*product.quality).value },
                    resilience: unsafe { (*product.resilience).value },
                    usage: unsafe { (*product.usage).value },
                },
            }
        }
        core::Class_id_Class_Plant => {
            let plant = unsafe { &*(el as *const core::InventoryElement as *const core::Plant) };
            ObjectData::Plant {
                data: PlantData {
                    base: convert_inv_el(el),
                    id: unsafe { (*plant.base)._base.id },
                    phase: plant.phase,
                    grown: plant.grown,
                    age: unsafe { (*plant._base.age).value },
                    max_age: unsafe { (*plant._base.max_age).value },
                },
            }
        }
        core::Class_id_Class_Animal => {
            let animal = unsafe { &*(el as *const core::InventoryElement as *const core::Animal) };
            ObjectData::Animal {
                data: AnimalData {
                    base: convert_inv_el(el),
                    id: unsafe { (*animal.base)._base.id },
                    age: unsafe { (*animal._base.age).value },
                    max_age: unsafe { (*animal._base.max_age).value },
                },
            }
        }
        _ => panic!(),
    }
}

fn convert_inv_el(el: &core::InventoryElement) -> InventoryElementData {
    unsafe {
        let location = if el.location.type_ == core::ItemLocationType_LOCATION_CHUNK {
            ItemLocationLol::Chunk {
                map_x: el.location.data.chunk.map_x,
                map_y: el.location.data.chunk.map_y,
                x: el.location.data.chunk.x,
                y: el.location.data.chunk.y,
            }
        } else {
            ItemLocationLol::Player {
                id: el.location.data.player.id as usize,
            }
        };
        InventoryElementData {
            uid: el.uid,
            el_type: el.c_id,
            location,
        }
    }
}

pub fn convert_item_location(location: &core::ItemLocation) -> ItemLocationLol {
    unsafe {
        if location.type_ == core::ItemLocationType_LOCATION_CHUNK {
            ItemLocationLol::Chunk {
                map_x: location.data.chunk.map_x,
                map_y: location.data.chunk.map_y,
                x: location.data.chunk.x,
                y: location.data.chunk.y,
            }
        } else {
            ItemLocationLol::Player {
                id: location.data.player.id as usize,
            }
        }
    }
}

pub fn convert_base_object(object: &core::Base) -> BaseObjectData {
    match object.c_id {
        core::Class_id_Class_BaseElement => {
            let base = unsafe { &*(object as *const core::Base as *const core::BaseElement) };
            BaseObjectData::Element {
                data: BaseElementData {
                    base: BaseData {
                        id: object.id,
                        c_id: object.c_id,
                    },
                    density: unsafe { (*base.density).value },
                    form: base.form,
                },
            }
        }
        core::Class_id_Class_BasePlant => {
            let base = unsafe { &*(object as *const core::Base as *const core::BasePlant) };
            BaseObjectData::Plant {
                data: BasePlantData {
                    base: BaseData {
                        id: object.id,
                        c_id: object.c_id,
                    },
                    flowers: base.flowers,
                    leaves: base.leaves,
                },
            }
        }
        core::Class_id_Class_BaseAnimal => {
            let base = unsafe { &*(object as *const core::Base as *const core::BaseAnimal) };
            BaseObjectData::Animal {
                data: BaseAnimalData {
                    base: BaseData {
                        id: object.id,
                        c_id: object.c_id,
                    },
                    carnivorous: base.carnivorous,
                    swimming: base.swimming,
                    flying: base.flying,
                },
            }
        }
        _ => {
            panic!("inexisting class");
        }
    }
}
