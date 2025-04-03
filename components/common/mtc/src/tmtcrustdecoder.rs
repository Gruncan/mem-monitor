use std::ffi::CString;
use std::os::raw::c_char;
use std::panic;
use crate::bindings::*;
use crate::{handle_mtc_result, MtcError, MtcResult};

pub enum TMtcKey {
    Malloc = 0x0,
    Calloc = 0x1,
    Realloc = 0x2,
    ReallocArray = 0x3,
    Free = 0x4,
    New = 0x5,
    NewNothrow = 0x6,
    NewArray = 0x7,
    NewArrayNothrow = 0x8,
    Delete = 0x9,
    DeleteSized = 0xa,
    DeleteNothrow = 0xb,
    DeleteArray = 0xc,
    DeleteArraySized = 0xd,
    DeleteArrayNothrow = 0xe,
    NewAlign = 0xf,
    NewArrayAlign = 0x10,
    DeleteAlign = 0x11,
    DeleteArrayAlign = 0x12,
}


pub struct TMtcObjectFfi {
    raw: TMtcObject,
    values: Vec<TMtcPointFfi>,
    owns_memory: bool,
}

pub struct TMtcPointFfi {
    raw: TMtcPoint,
}


impl TMtcObjectFfi {

    pub fn new() -> MtcResult<Self> {
        let mut raw = TMtcObject{
            points: std::ptr::null_mut(),
            size: 0,
            _file_length: 0,
            _allocation_size: 0,
            is_collapsable: 0
        };

        let result = panic::catch_unwind(|| unsafe {
            createTMtcObject(&mut raw);
            ()
        });

        handle_mtc_result!(result, TMtcObjectFfi{raw, owns_memory: true, values: Vec::new()},
                            initialisation_failure)
    }

    pub fn get_size(&self) -> usize {
        self.raw.size as usize
    }

    pub fn get_is_collapsable(&self) -> bool {
        if self.raw.is_collapsable == 1 {
            true
        } else {
            false
        }
    }

    pub fn decode(&mut self, filename: &str) -> MtcResult<()> {
        let c_ptr: *const c_char = CString::new(filename).expect("CString::new failed").into_raw();
        let result = panic::catch_unwind(|| unsafe {
            decode_tmtc(c_ptr, &mut self.raw);
            self.values = Vec::with_capacity(self.raw.size as usize);
            for i in 0..self.raw.size {
                self.values.push(TMtcPointFfi {raw: *self.raw.points.wrapping_add(i as usize)});
            }
        });

        handle_mtc_result!(result, (), decode_failure)
    }

    pub fn get_points(&mut self) -> &Vec<TMtcPointFfi> {
        &self.values
    }

}

impl Drop for TMtcObjectFfi {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                destroyTMtcObject(&mut self.raw);
            }
            self.owns_memory = false;
        }
    }
}

impl TMtcKey {
    pub fn from_int(value: u8) -> Option<Self> {
        match value {
            0x0 => Some(Self::Malloc),
            0x1 => Some(Self::Calloc),
            0x2 => Some(Self::Realloc),
            0x3 => Some(Self::ReallocArray),
            0x4 => Some(Self::Free),
            0x5 => Some(Self::New),
            0x6 => Some(Self::NewNothrow),
            0x7 => Some(Self::NewArray),
            0x8 => Some(Self::NewArrayNothrow),
            0x9 => Some(Self::Delete),
            0xa => Some(Self::DeleteSized),
            0xb => Some(Self::DeleteNothrow),
            0xc => Some(Self::DeleteArray),
            0xd => Some(Self::DeleteArraySized),
            0xe => Some(Self::DeleteArrayNothrow),
            0xf => Some(Self::NewAlign),
            0x10 => Some(Self::NewArrayAlign),
            0x11 => Some(Self::DeleteAlign),
            0x12 => Some(Self::DeleteArrayAlign),
            _ => None,
        }
    }
}

impl TMtcPointFfi {

    pub fn get_key(&self) -> Option<TMtcKey> {
        TMtcKey::from_int(self.raw.key)
    }

    pub fn get_length(&self) -> u8 {
        self.raw.length
    }

    pub fn get_time_offset(&self) -> u32 {
        self.raw.time_offset
    }

    pub fn get_values(&self) -> MtcResult<Vec<u64>> {
        let mut values = Vec::with_capacity(self.get_length() as usize);
        let result = panic::catch_unwind(|| unsafe {
            for i in 0..self.get_length() {
                values.push(*self.raw.values.wrapping_add(i as usize))
            }
        });
        handle_mtc_result!(result, values, decode_failure)
    }

}