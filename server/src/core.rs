#![allow(warnings)]
include!(concat!(env!("OUT_DIR"), "/core_bindings.rs"));

include!("../../core/alchemist/item_location.rs");

use serde::{ser::SerializeTuple, Serialize};
use std::ffi::{CStr, CString};

impl std::fmt::Debug for SerializableCString {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str(unsafe { CStr::from_ptr(self.str_).to_str().unwrap() })
    }
}
impl serde::Serialize for BaseElementServer {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        self._base.serialize(serializer)
    }
}

impl serde::Serialize for SerializableCString {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let bytes = unsafe { CStr::from_ptr(self.str_).to_bytes_with_nul() };
        bytes.serialize(serializer)
    }
}

impl serde::Serialize for Base {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(3)?;
        state.serialize_element(&self.c_id)?;
        state.serialize_element(&self.id)?;
        state.serialize_element(&self.name)?;
        state.end()
    }
}

impl serde::Serialize for SerializablePointer<BaseElement> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        i32::serialize(unsafe { &(*self.ptr)._base.id }, serializer)
    }
}

impl serde::Serialize for SerializablePointer<BasePlant> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        i32::serialize(unsafe { &(*self.ptr)._base.id }, serializer)
    }
}

impl serde::Serialize for SerializablePointer<BaseAnimal> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        i32::serialize(unsafe { &(*self.ptr)._base.id }, serializer)
    }
}

impl serde::Serialize for InventoryElement {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(4)?;
        state.serialize_element(&self.c_id)?;
        state.serialize_element(&self.uid)?;
        state.serialize_element(&self.mass)?;
        state.serialize_element(&self.location)?;
        state.end()
    }
}

// impl serde::Serialize for Property {
//     fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
//     where
//         S: serde::Serializer,
//     {
//         let mut state = serializer.serialize_tuple(2)?;
//         let bytes = unsafe { CStr::from_ptr(self.name).to_bytes_with_nul() };
//         state.serialize_element(&bytes)?;
//         state.serialize_element(&self.value)?;
//         state.end()
//         // u32::serialize(&self.value, serializer)
//     }
// }
