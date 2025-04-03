use std::ffi::CString;
use crate::bindings::*;
use std::os::raw::{c_char};

pub struct MtcObjectFfi {
    raw: MtcObject,
    owns_memory: bool,
}


impl MtcObjectFfi {

    pub fn new() -> Self {
        let mut raw = MtcObject{
            point_map: std::ptr::null_mut(),
            times: std::ptr::null_mut(),
            size: 0,
            version: 0,
            _times_length: 0,
            _alloc_size_points: 0,
            _alloc_size_times: 0,
            file_length: 0,
            _key_size: 0,
        };

        unsafe {
            createMtcObject(&mut raw);
        }

        MtcObjectFfi {
            raw,
            owns_memory: true,
        }
    }

    pub fn decode(&mut self, filename: &str) {
        let c_ptr: *const c_char = CString::new(filename).expect("CString::new failed").into_raw();
        unsafe {
            decode(c_ptr, &mut self.raw);
        }
    }

    pub fn get_version(&self) -> u8 {
        self.raw.version
    }
}

impl Drop for MtcObjectFfi {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                destroyMtcObject(&mut self.raw);
            }
        }
    }
}

