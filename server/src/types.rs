use crate::core;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Serialize, Debug)]
pub struct LocationUpdateData {
    pub id: core::NetworkObject,
    pub old: core::ItemLocation,
    pub new: core::ItemLocation,
}

#[repr(C)]
#[derive(Serialize, Debug)]
#[allow(dead_code)]
pub enum ObjectData {
    InvElement { data: core::InventoryElement },
    Element { data: core::Element },
    Plant { data: core::Plant },
    Animal { data: core::Animal },
    Ingredient { data: core::Ingredient },
    Product { data: core::Product },
    Scroll { data: core::Scroll },
    Player { data: core::Player },
    Npc { data: core::Npc },
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct BaseData {
    pub id: i32,
    pub c_id: core::Class_id,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct BaseElementData {
    pub base: BaseData,
    pub density: u32,
    pub form: core::Form,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct BasePlantData {
    pub base: BaseData,
    pub flowers: bool,
    pub leaves: bool,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct BaseAnimalData {
    pub base: BaseData,
    pub carnivorous: bool,
    pub swimming: bool,
    pub flying: bool,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub enum BaseObjectData {
    Element { data: BaseElementData },
    Plant { data: BasePlantData },
    Animal { data: BaseAnimalData },
}
