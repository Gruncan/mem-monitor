mod mtcrustdecoder;

use std::os::raw::{c_char, c_uchar};
use libc::FILE;

pub type c_void = std::ffi::c_void;

pub type mk_size_t = u8;
pub type mtc_point_size_t = u32;
pub type byte_t = c_uchar;

#[repr(C)]
pub struct CMtcPoint {
    pub time_offset: u16,
    pub value: mtc_point_size_t,
    pub repeated: u64
}

#[repr(C)]
pub struct CMtcPointMap {
    pub points: *mut CMtcPoint,
    pub length: u16
}

#[repr(C)]
pub struct CMtcTime {
    pub time_offset: u16,
    pub repeated: u64
}

#[repr(C)]
pub struct CMtcObject {
    pub point_map: *mut CMtcPointMap,
    pub times: *mut CMtcTime,
    pub size: u64,
    pub version: u8,
    _times_length: u64,
    _alloc_size_points: u64,
    _alloc_size_times: u64,
    _file_length: u64,
    _key_size: mk_size_t
}


#[repr(C)]
pub struct CTMtcPoint {
    pub key: u8,
    pub length: u8,
    pub time_offset: u32,
    pub values: *u64
}

#[repr(C)]
pub struct CTMtcObject {
    pub points: *mut CTMtcPoint,
    pub size: u64,
    _file_length: u64,
    _allocation_size: u64,
    pub is_collapsable: c_char
}

#[repr(C)]
pub struct CTMtcStream {
    pub object: *mut CTMtcObject,
    _next: *mut CTMtcObject,
    index: u16,
    offset: u64,
    fp: *mut FILE,
    _chunk_size: u16,
    _read_buffer: *mut byte_t,
    flipper: c_char,
}


#[link(name = "libmtc_decoder")]
extern "C" {

    pub fn c_decode(filename: *const c_char, object: *mut CMtcObject);

    pub fn c_createMtcObject(object: *mut CMtcObject);

    pub fn c_destroyMtcObject(object: *mut CMtcObject);

    pub fn c_queryDecodeProgress(object: *mut CMtcObject) -> u8;

}