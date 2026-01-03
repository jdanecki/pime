#![allow(warnings)]
include!("core_bindings.rs");

include!("../../core/alchemist/item_location.rs");

use serde::{
    ser::{SerializeSeq, SerializeTuple},
    Serialize,
};
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

impl<T> serde::Serialize for SerializablePointer<T> {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        self.id.serialize(serializer)
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

impl serde::Serialize for InventoryElement {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::Serializer,
    {
        let mut state = serializer.serialize_tuple(3)?;
        unsafe {
            state.serialize_element(&self._base.get_cid())?;
            state.serialize_element(&self._base.get_uid())?;
        }
        state.serialize_element(&self.location)?;
        state.end()
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
        let mut state = serializer.serialize_seq(Some(data.len() + 1))?;
        state.serialize_element(&data.len());
        for d in data {
            state.serialize_element(&d)?;
        }
        state.end()
    }
}
