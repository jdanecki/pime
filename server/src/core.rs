#![allow(warnings)]
include!("core_bindings.rs");

include!("../../core/alchemist/item_location.rs");

use serde::{ser::SerializeTuple, Serialize};
use std::{
    ffi::{CStr, CString},
    ptr::addr_of_mut,
};

impl std::fmt::Debug for SerializableCString {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str(unsafe { CStr::from_ptr(self.str_).to_str().unwrap() })
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
        state.serialize_element(&self._base.c_id)?;
        state.serialize_element(&self._base.uid)?;
        state.serialize_element(&self.name)?;
        state.end()
    }
}

impl serde::Serialize for SerializablePointer<NetworkObject> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(2)?;
        state.serialize_element(unsafe { &(*self.ptr).c_id })?;
        state.serialize_element(unsafe { &(*self.ptr).uid })?;
        state.end()
    }
}

impl serde::Serialize for SerializablePointer<InventoryElement> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(2)?;
        state.serialize_element(unsafe { &(*self.ptr)._base.c_id })?;
        state.serialize_element(unsafe { &(*self.ptr)._base.uid })?;
        state.end()
    }
}

impl serde::Serialize for SerializablePointer<BaseElement> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        usize::serialize(unsafe { &(*self.ptr)._base._base.uid }, serializer)
    }
}

impl serde::Serialize for SerializablePointer<BasePlant> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        usize::serialize(unsafe { &(*self.ptr)._base._base.uid }, serializer)
    }
}

impl serde::Serialize for SerializablePointer<BaseAnimal> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        usize::serialize(unsafe { &(*self.ptr)._base._base.uid }, serializer)
    }
}

impl serde::Serialize for SerializablePointer<Base> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(2)?;
        unsafe {
            state.serialize_element(&(*self.ptr)._base.c_id)?;
            state.serialize_element(&(*self.ptr)._base.uid)?;
        }
        state.end()
    }
}

impl serde::Serialize for InventoryElement {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(3)?;
        unsafe {
            state.serialize_element(&self.get_cid())?;
            state.serialize_element(&self.get_uid())?;
        }
        state.serialize_element(&self.location)?;
        state.end()
    }
}

// impl serde::Serialize for Player {
//     fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
//     where
//         S: serde::Serializer,
//     {
//         let mut state = serializer.serialize_tuple(6)?;
//         unsafe {
//             state.serialize_element(&self._base.get_uid())?;
//         }
//         state.serialize_element(&self.name)?;
//         state.serialize_element(&self._base.location)?;
//         state.serialize_element(&self.thirst)?;
//         state.serialize_element(&self.hunger)?;
//         state.serialize_element(&self.nutrition)?;
//         state.end()
//     }
// }

impl serde::Serialize for SerializablePointer<Player> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        if self.ptr != std::ptr::null_mut() {
            usize::serialize(unsafe { &(*self.ptr)._base.get_uid() }, serializer)
        } else {
            usize::serialize(&0, serializer)
        }
    }
}

impl serde::Serialize for SerializablePointer<Clan> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        usize::serialize(unsafe { &(*self.ptr)._base.uid }, serializer)
    }
}

impl serde::Serialize for ElementsList {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut data = vec![];
        let len = self.nr_elements as usize;
        unsafe {
            let mut iter = self.begin();
            while iter.equal(&self.end() as *const ElementsListIterator) {
                let ptr = iter.get();
                let id = if ptr == std::ptr::null_mut() {
                    NetworkObject { c_id: 0, uid: 0 }
                } else {
                    unsafe {
                        NetworkObject {
                            c_id: (*ptr)._base.c_id,
                            uid: (*ptr)._base.uid,
                        }
                    }
                };
                data.push(SerializablePointer {
                    _phantom_0: std::marker::PhantomData,
                    ptr: iter.get(),
                    id,
                });
                iter.next();
            }
        }
        assert!(data.len() == len);
        println!("LIST HAS {len} ELEMENTS");
        if len > 0 {
            let mut state = serializer.serialize_tuple(data.len())?;
            for d in data {
                state.serialize_element(&d)?;
            }
            state.end()
        } else {
            usize::serialize(&0, serializer)
        }
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
