use crate::core;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Serialize, Deserialize, Copy, Clone, Debug)]
pub enum ItemLocationLol {
    Chunk {
        map_x: i32,
        map_y: i32,
        x: i32,
        y: i32,
    },
    Player {
        id: usize,
    },
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct LocationUpdateData {
    pub id: usize,
    pub old: ItemLocationLol,
    pub new: ItemLocationLol,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct InventoryElementData {
    pub uid: usize,
    pub el_type: core::Class_id,
    pub location: ItemLocationLol,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct ElementData {
    pub base: InventoryElementData,
    pub id: i32,
    pub sharpness: u32,
    pub smoothness: u32,
    pub mass: u32,
    pub length: u32,
    pub width: u32,
    pub height: u32,
    pub volume: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct IngredientData {
    pub base: InventoryElementData,
    pub id: core::Ingredient_id,
    pub quality: u32,
    pub resilience: u32,
    pub usage: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct ProductData {
    pub base: InventoryElementData,
    pub id: core::Product_id,
    pub quality: u32,
    pub resilience: u32,
    pub usage: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct PlantData {
    pub base: InventoryElementData,
    pub id: i32,
    pub phase: core::Plant_phase,
    pub grown: bool,
    pub age: u32,
    pub max_age: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct AnimalData {
    pub base: InventoryElementData,
    pub id: i32,
    pub age: u32,
    pub max_age: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub enum ObjectData {
    InvElement { data: InventoryElementData },
    Element { data: ElementData },
    Ingredient { data: IngredientData },
    Product { data: ProductData },
    Plant { data: PlantData },
    Animal { data: AnimalData },
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

#[no_mangle]
extern "C" fn foo() -> ObjectData {
    ObjectData::InvElement {
        data: InventoryElementData {
            uid: 123,
            el_type: 1,
            location: ItemLocationLol::Player { id: 1 },
        },
    }
}

impl serde::Serialize for core::BaseAnimal {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        BaseAnimalData {
            base: BaseData {
                id: self._base.id,
                c_id: self._base.c_id,
            },
            carnivorous: self.carnivorous,
            swimming: self.swimming,
            flying: self.flying,
        }
        .serialize(serializer)
    }
}

impl<'de> serde::Deserialize<'de> for core::BaseAnimal {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let d = BaseAnimalData::deserialize(deserializer)?;
        Ok(unsafe { core::BaseAnimal::new(d.base.id) })
    }
}

impl serde::Serialize for core::BaseElement {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        BaseElementData {
            base: BaseData {
                id: self._base.id,
                c_id: self._base.c_id,
            },
            density: unsafe { (*self.density).value },
            form: self.form,
        }
        .serialize(serializer)
    }
}

impl<'de> serde::Deserialize<'de> for core::BaseElement {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let d = BaseElementData::deserialize(deserializer)?;
        Ok(unsafe { core::BaseElement::new(d.form, d.base.id) })
    }
}

impl serde::Serialize for core::BasePlant {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        BasePlantData {
            base: BaseData {
                id: self._base.id,
                c_id: self._base.c_id,
            },
            flowers: self.flowers,
            leaves: self.leaves,
        }
        .serialize(serializer)
    }
}

impl<'de> serde::Deserialize<'de> for core::BasePlant {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::Deserializer<'de>,
    {
        let d = BasePlantData::deserialize(deserializer)?;
        Ok(unsafe { core::BasePlant::new(d.base.id) })
    }
}
