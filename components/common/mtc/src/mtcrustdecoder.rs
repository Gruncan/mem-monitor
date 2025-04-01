use std::ffi::CString;
use crate::{c_createMtcObject, c_decode, c_destroyMtcObject, CMtcObject};
use std::os::raw::{c_char};

pub struct MtcObject {
    raw: CMtcObject,
    owns_memory: bool,
}


impl MtcObject {

    pub fn new() -> Self {
        let mut raw = CMtcObject{
            point_map: std::ptr::null_mut(),
            times: std::ptr::null_mut(),
            size: 0,
            version: 0,
            _times_length: 0,
            _alloc_size_points: 0,
            _alloc_size_times: 0,
            _file_length: 0,
            _key_size: 0,
        };

        unsafe {
            c_createMtcObject(&mut raw);
        }

        MtcObject {
            raw,
            owns_memory: true,
        }
    }

    pub fn decode(&mut self, filename: &str) {
        let c_ptr: *const c_char = CString::new(filename).expect("CString::new failed").into_raw();
        unsafe {
            c_decode(c_ptr, &mut self.raw);
        }
    }

    pub fn get_version(&self) -> u8 {
        self.raw.version
    }
}

impl Drop for MtcObject {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                c_destroyMtcObject(&mut self.raw);
            }
        }
    }
}

