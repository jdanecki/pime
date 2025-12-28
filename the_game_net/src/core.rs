#![allow(warnings)]
include!(concat!("core_bindings.rs"));

include!("../../core/alchemist/item_location.rs");

use std::ffi::{CStr, CString};
use std::fmt;
use std::hash::Hash;

use serde::de::{SeqAccess, Visitor};
use serde::{Deserialize, Deserializer};

use crate::get_base_animal;
use crate::get_base_element;
use crate::get_base_plant;
use crate::{get_base, get_object_by_id};

impl<T> SerializablePointer<T> {
    pub fn new(p: *mut T) -> Self {
        Self {
            _phantom_0: std::marker::PhantomData,
            ptr: p,
            id: NetworkObject { c_id: 0, uid: 0 },
        }
    }
    pub fn from_id(id: NetworkObject) -> Self {
        Self {
            _phantom_0: std::marker::PhantomData,
            ptr: std::ptr::null_mut(),
            id,
        }
    }
}

impl<'de, T> serde::Deserialize<'de> for SerializablePointer<T> {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        let no = NetworkObject::deserialize(deserializer)?;

        Ok(SerializablePointer::from_id(no))
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
                let id: usize = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(1, &self))?;
                let name: &[u8] = seq
                    .next_element()?
                    .ok_or_else(|| serde::de::Error::invalid_length(2, &self))?;
                let name =
                    CString::from_vec_with_nul(name.to_vec()).expect("BAD property name received");
                Ok(unsafe { Base::new(id as i32, c_id, name.into_raw()) })
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
                Ok(unsafe { InventoryElement::new(c_id, id, location) })
            }
        }

        deserializer.deserialize_tuple(4, InventoryElementVisitor)
    }
}

impl<'de> serde::Deserialize<'de> for ElementsList {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        struct ElementsListVisitor;

        impl<'de> Visitor<'de> for ElementsListVisitor {
            type Value = ElementsList;

            fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
                formatter.write_str("struct ElementsList")
            }

            fn visit_seq<V>(self, mut seq: V) -> Result<ElementsList, V::Error>
            where
                V: SeqAccess<'de>,
            {
                unsafe {
                    let mut a = ElementsList::new1();
                    if let Some(num) = seq.next_element()? {
                        let num: usize = num;
                        println!("list has {num} elements");
                        for _ in 0..num {
                            if let Some(o) = seq.next_element()? {
                                let mut o: NetworkObject = o;
                                let mut le = ListElement::new1(&mut o as *mut NetworkObject);
                                a.add(&mut le as *mut ListElement);
                            } else {
                                panic!("WTF");
                            }
                        }
                        return Ok(a);
                    }
                    panic!("failed to deserialize");
                }
            }
        }

        deserializer.deserialize_tuple(6, ElementsListVisitor)
    }
}

impl Hash for NetworkObject {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        (self.c_id, self.uid).hash(state);
    }
}
