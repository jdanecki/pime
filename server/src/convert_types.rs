use crate::core;
use crate::types::*;

pub fn convert_to_data(el: &core::InventoryElement) -> ObjectData {
    match el.c_id {
        core::Class_id_Class_Element => {
            let element =
                unsafe { &*(el as *const core::InventoryElement as *const core::Element) };
            ObjectData::Element { data: *element }
        }
        core::Class_id_Class_Scroll => {
            let scroll = unsafe { &*(el as *const core::InventoryElement as *const core::Scroll) };
            ObjectData::Scroll { data: *scroll }
        }
        core::Class_id_Class_Ingredient => {
            let ingredient =
                unsafe { &*(el as *const core::InventoryElement as *const core::Ingredient) };
            ObjectData::Ingredient { data: *ingredient }
        }
        core::Class_id_Class_Product => {
            let product =
                unsafe { &*(el as *const core::InventoryElement as *const core::Product) };
            ObjectData::Product { data: *product }
        }
        core::Class_id_Class_Plant => {
            let plant = unsafe { &*(el as *const core::InventoryElement as *const core::Plant) };
            ObjectData::Plant { data: *plant }
        }
        core::Class_id_Class_Animal => {
            let animal = unsafe { &*(el as *const core::InventoryElement as *const core::Animal) };
            ObjectData::Animal { data: *animal }
        }
        core::Class_id_Class_Player => {
            let player = unsafe { &*(el as *const core::InventoryElement as *const core::Player) };
            ObjectData::Player { data: *player }
        }
        _ => {
            println!("WRONG CLASS ID {}", el.c_id);
            panic!();
        }
    }
}

// fn convert_inv_el(el: &core::InventoryElement) -> InventoryElementData {
//     unsafe {
//         let location = if el.location.type_ == core::ItemLocationType_LOCATION_CHUNK {
//             ItemLocationLol::Chunk {
//                 map_x: el.location.data.chunk.map_x,
//                 map_y: el.location.data.chunk.map_y,
//                 x: el.location.data.chunk.x,
//                 y: el.location.data.chunk.y,
//             }
//         } else {
//             ItemLocationLol::Player {
//                 id: el.location.data.player.id as usize,
//             }
//         };
//         InventoryElementData {
//             uid: el.uid,
//             el_type: el.c_id,
//             location,
//         }
//     }
// }

// pub fn convert_item_location(location: &core::ItemLocation) -> ItemLocationLol {
//     unsafe {
//         if location.type_ == core::ItemLocationType_LOCATION_CHUNK {
//             ItemLocationLol::Chunk {
//                 map_x: location.data.chunk.map_x,
//                 map_y: location.data.chunk.map_y,
//                 x: location.data.chunk.x,
//                 y: location.data.chunk.y,
//             }
//         } else {
//             ItemLocationLol::Player {
//                 id: location.data.player.id as usize,
//             }
//         }
//     }
// }

// pub fn convert_base_object(object: &core::Base) -> BaseObjectData {
//     match object.c_id {
//         core::Class_id_Class_BaseElement => {
//             let base = unsafe { &*(object as *const core::Base as *const core::BaseElement) };
//             BaseObjectData::Element {
//                 data: BaseElementData {
//                     base: BaseData {
//                         id: object.id,
//                         c_id: object.c_id,
//                     },
//                     density: unsafe { (*base.density).value },
//                     form: base.form,
//                 },
//             }
//         }
//         core::Class_id_Class_BasePlant => {
//             let base = unsafe { &*(object as *const core::Base as *const core::BasePlant) };
//             BaseObjectData::Plant {
//                 data: BasePlantData {
//                     base: BaseData {
//                         id: object.id,
//                         c_id: object.c_id,
//                     },
//                     flowers: base.flowers,
//                     leaves: base.leaves,
//                 },
//             }
//         }
//         core::Class_id_Class_BaseAnimal => {
//             let base = unsafe { &*(object as *const core::Base as *const core::BaseAnimal) };
//             BaseObjectData::Animal {
//                 data: BaseAnimalData {
//                     base: BaseData {
//                         id: object.id,
//                         c_id: object.c_id,
//                     },
//                     carnivorous: base.carnivorous,
//                     swimming: base.swimming,
//                     flying: base.flying,
//                 },
//             }
//         }
//         _ => {
//             panic!("inexisting class");
//         }
//     }
// }
