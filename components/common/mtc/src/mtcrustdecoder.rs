use std::ffi::CString;
use crate::bindings::*;
use std::os::raw::{c_char};




pub struct MtcObjectFfi {
    raw: MtcObject,
    values: Vec<MtcPointFfi>,
    owns_memory: bool,
}

pub struct MtcPointFfi {
    raw: MtcPoint,
}

pub struct MtcTimeFfi {
    raw: MtcTime,
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
            values: Vec::new(),
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

    pub fn get_size(&self) -> u64 {
        self.raw.size
    }
}

impl Drop for MtcObjectFfi {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                destroyMtcObject(&mut self.raw);
            }
            self.owns_memory = false;
        }
    }
}

trait MtcPointVariant {

    fn get_time_offset(&self) -> u16;

    fn get_repeated(&self) -> u64;

}


impl MtcPointVariant for MtcPointFfi {


    fn get_time_offset(&self) -> u16 {
        unsafe {
            *self.raw.time_offset
        }
    }

    fn get_repeated(&self) -> u64 {
        self.raw.repeated
    }
}

impl MtcPointFfi {
    pub fn get_value(&self) -> mtc_point_size_t {
        self.raw.value
    }
}

impl MtcPointVariant for MtcTimeFfi {


    fn get_time_offset(&self) -> u16 {
        unsafe {
            *self.raw.time_offset
        }
    }

    fn get_repeated(&self) -> u64 {
        self.raw.repeated
    }

}


