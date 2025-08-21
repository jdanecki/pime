#![allow(warnings)]
include!(concat!(env!("OUT_DIR"), "/core_bindings.rs"));

include!("../../core/alchemist/item_location.rs");

use std::ffi::{CStr, CString};
use std::fmt;

use serde::de::{SeqAccess, Visitor};
use serde::{Deserialize, Deserializer};

use crate::get_base_animal;
use crate::get_base_element;
use crate::get_base_plant;
use crate::get_base;

impl<T> SerializablePointer<T> {
    pub fn new(p: *mut T) -> Self {
        Self {
            _phantom_0: std::marker::PhantomData,
            ptr: p,
        }
    }
}

impl std::fmt::Debug for SerializableCString {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str(unsafe { CStr::from_ptr(self.str_).to_str().unwrap() })
    }
}

impl<'de> serde::Deserialize<'de> for Base {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        struct BaseVisitor;

        impl<'de> Visitor<'de> for BaseVisitor {
            type Value = Base;

            fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
                formatter.write_str("struct Base")
            }

            fn visit_seq<V>(self, mut seq: V) -> Result<Base, V::Error>
            where
                V: SeqAccess<'de>,
            {
                let c_id = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(0, &self))?;
                let id = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(1, &self))?;
                let name: &[u8] = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(2, &self))?;
                let name =
                    CString::from_vec_with_nul(name.to_vec()).expect("BAD property name received");
                Ok(unsafe { Base::new(id, c_id, name.into_raw()) })
            }
        }

        deserializer.deserialize_tuple(3, BaseVisitor)
    }
}

impl<'de> serde::Deserialize<'de> for SerializableCString {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        // struct Vis;

        // impl<'de> Visitor<'de> for Vis {
        //     type Value = SerializableCString;

        //     fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        //         formatter.write_str("struct SerializableCString")
        //     }

        //     fn visit_seq<V>(self, mut seq: V) -> Result<SerializableCString, V::Error>
        //     where
        //         V: SeqAccess<'de>,
        //     {
        //         let c_id = seq
        //             .next_element()?
        //             .ok_or_else(|| serde::de::Error::invalid_length(0, &self))?;
        //         let id = seq
        //             .next_element()?
        //             .ok_or_else(|| serde::de::Error::invalid_length(1, &self))?;
        //         let name: &[u8] = seq
        //             .next_element()?
        //             .ok_or_else(|| serde::de::Error::invalid_length(2, &self))?;
        //         let name =
        //             CString::from_vec_with_nul(name.to_vec()).expect("BAD property name received");
        //         // Ok(unsafe { Base::new(id, c_id, name.into_raw()) })

        //         serde::de::Error::invalid_length(2, &self)
        //     }
        // }
        // deserializer.deserialize_byte_buf()
        let name: &[u8] = Deserialize::deserialize(deserializer)?;
        Ok(unsafe {
            SerializableCString::new(
                CString::from_vec_with_nul(name.to_vec())
                    .unwrap()
                    .into_raw(),
            )
        })
    }
}

impl<'de> serde::Deserialize<'de> for SerializablePointer<BaseElement> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        let id = i32::deserialize(deserializer)?;

        Ok(SerializablePointer::new(get_base_element(id)))
    }
}

impl<'de> serde::Deserialize<'de> for SerializablePointer<BasePlant> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        let id = i32::deserialize(deserializer)?;

        Ok(SerializablePointer::new(get_base_plant(id)))
    }
}

impl<'de> serde::Deserialize<'de> for SerializablePointer<BaseAnimal> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        let id = i32::deserialize(deserializer)?;

        Ok(SerializablePointer::new(get_base_animal(id)))
    }
}


impl<'de> serde::Deserialize<'de> for SerializablePointer<Base> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        struct BaseVisitor;

        impl<'de> Visitor<'de> for BaseVisitor {
            type Value = SerializablePointer<Base>;

            fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
                formatter.write_str("struct SerializablePointer<Base>")
            }

            fn visit_seq<V>(self, mut seq: V) -> Result<SerializablePointer<Base>, V::Error>
            where
                V: SeqAccess<'de>,
            {
                let c_id = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(0, &self))?;
                let id = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(1, &self))?;
                Ok(unsafe { SerializablePointer::new(get_base(c_id, id)) })
            }
        }

        deserializer.deserialize_tuple(2, BaseVisitor)
     }
}


impl<'de> serde::Deserialize<'de> for InventoryElement {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        struct InventoryElementVisitor;

        impl<'de> Visitor<'de> for InventoryElementVisitor {
            type Value = InventoryElement;

            fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
                formatter.write_str("struct InventoryElement")
            }

            fn visit_seq<V>(self, mut seq: V) -> Result<InventoryElement, V::Error>
            where
                V: SeqAccess<'de>,
            {
                let c_id = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(0, &self))?;
                let id = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(1, &self))?;
                let location = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(2, &self))?;
                let checked = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(3, &self))?;
                Ok(unsafe { InventoryElement::new(c_id, id, location, checked) })
            }
        }

        deserializer.deserialize_tuple(4, InventoryElementVisitor)
    }
}

// impl<'de> serde::Deserialize<'de> for Property {
//     fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
//     where
//         D: Deserializer<'de>,
//     {
//         struct PropertyVisitor;

//         impl<'de> Visitor<'de> for PropertyVisitor {
//             type Value = Property;

//             fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
//                 formatter.write_str("struct Property")
//             }

//             fn visit_seq<V>(self, mut seq: V) -> Result<Property, V::Error>
//             where
//                 V: SeqAccess<'de>,
//             {
//                 let name: &[u8] = seq
//                     .next_element()?
//                     .ok_or_else(|| serde::de::Error::invalid_length(0, &self))?;
//                 let value = seq
//                     .next_element()?
//                     .ok_or_else(|| serde::de::Error::invalid_length(123, &self))?;
//                 let name =
//                     CString::from_vec_with_nul(name.to_vec()).expect("BAD property name received");

//                 Ok(unsafe {
//                     Property {
//                         name: name.into_raw(),
//                         value,
//                     }
//                 })
//             }
//         }

//         deserializer.deserialize_tuple(2, PropertyVisitor)

//         // let value = u32::deserialize(deserializer)?;
//         // Ok(Property {
//         //     name: std::ptr::null(),
//         //     value,
//         // })
//     }
// }
