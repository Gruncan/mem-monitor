use std::ffi::CString;
use std::os::raw::c_char;
use std::panic;
use std::panic::AssertUnwindSafe;
use crate::bindings::*;
use crate::*;

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

        let result = panic::catch_unwind(AssertUnwindSafe(|| unsafe {
            createTMtcObject(&mut raw);
            ()
        }));

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
        let result = panic::catch_unwind(AssertUnwindSafe(|| unsafe {
            decode_tmtc(c_ptr, &mut self.raw);
            self.values = Vec::with_capacity(self.raw.size as usize);
            for i in 0..self.raw.size {
                let raw_value: TMtcPoint = get_mtc_struct_from_ptr!(self.raw.points.wrapping_add(i as usize));
                self.values.push(TMtcPointFfi {raw: raw_value});
            }
        }));

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
        let result = panic::catch_unwind(AssertUnwindSafe(|| unsafe {
            for i in 0..self.get_length() {
                let value: u64 = get_mtc_struct_from_ptr!(self.raw.values.wrapping_add(i as usize));
                values.push(value);
            }
        }));
        handle_mtc_result!(result, values, decode_failure)
    }

    pub fn convert(self) -> MtcResult<Box<dyn AllocationPoint>> {
        let key_option = self.get_key();
        if key_option.is_none() {
            return Err(MtcError::data_read_failure("Invalid key".to_string()));
        }
        let key = key_option.unwrap();
        let values = self.get_values()?;
        let time_offset = self.get_time_offset() as u64;
        let wrapped_data: Box<dyn AllocationPoint> = match key {
            TMtcKey::Malloc => Malloc::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::Calloc => Calloc::new_box(time_offset, values[2], values[0] as usize, values[1] as usize),
            TMtcKey::Realloc => Realloc::new_box(time_offset, values[0], values[1] as usize, values[2].into()),
            TMtcKey::ReallocArray => ReallocArray::new_box(time_offset, values[0], values[1] as usize, values[2] as usize, 0.into()),
            TMtcKey::Free => Free::new_box(time_offset, values[0]),
            TMtcKey::New => New::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::NewNothrow => NewNothrow::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::NewArray => NewArray::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::NewArrayNothrow => NewArrayNothrow::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::Delete => Delete::new_box(time_offset, values[0]),
            TMtcKey::DeleteSized => DeleteSized::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::DeleteNothrow => DeleteNothrow::new_box(time_offset, values[0]),
            TMtcKey::DeleteArray => DeleteArray::new_box(time_offset, values[0]),
            TMtcKey::DeleteArraySized => DeleteArraySized::new_box(time_offset, values[0], values[1] as usize),
            TMtcKey::DeleteArrayNothrow => DeleteArrayNothrow::new_box(time_offset, values[0]),
            TMtcKey::NewAlign => NewAlign::new_box(time_offset, values[0], values[1] as usize, values[2].into()),
            TMtcKey::NewArrayAlign => NewArrayAlign::new_box(time_offset, values[0], values[1] as usize, values[2].into()),
            TMtcKey::DeleteAlign => DeleteAlign::new_box(time_offset, values[0], values[1].into()),
            TMtcKey::DeleteArrayAlign => DeleteArrayAlign::new_box(time_offset, values[0], values[1].into()),
        };
        Ok(wrapped_data)
    }

}

#[derive(Clone, Copy)]
struct MemoryAddress {
    address: u64,
}

impl From<u64> for MemoryAddress {
    fn from(address: u64) -> Self {
        MemoryAddress{address}
    }
}

#[derive(Clone, Copy)]
struct MemoryAlignment {
    alignment: u64,
}

impl From<u64> for MemoryAlignment {
    fn from(alignment: u64) -> Self {
        MemoryAlignment{alignment}
    }
}

struct MemoryAllocationMeta {
    timestamp: u64,
    ptr: MemoryAddress,
    key: TMtcKey,
}

struct Malloc {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct Calloc {
    allocation_meta: MemoryAllocationMeta,
    nmemb: usize,
    size: usize,
}

struct Realloc {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
    new_ptr: MemoryAddress,
}

struct ReallocArray {
    allocation_meta: MemoryAllocationMeta,
    nmemb: usize,
    size: usize,
    new_ptr: MemoryAddress,
}

struct Free {
    allocation_meta: MemoryAllocationMeta,
}

struct New {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct NewNothrow {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct NewArray {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct NewArrayNothrow {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct Delete {
    allocation_meta: MemoryAllocationMeta,
}

struct DeleteSized {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct DeleteNothrow {
    allocation_meta: MemoryAllocationMeta,
}

struct DeleteArray {
    allocation_meta: MemoryAllocationMeta,
}

struct DeleteArraySized {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
}

struct DeleteArrayNothrow {
    allocation_meta: MemoryAllocationMeta,
}

struct NewAlign {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
    align: MemoryAlignment
}

struct NewArrayAlign {
    allocation_meta: MemoryAllocationMeta,
    size: usize,
    align: MemoryAlignment
}

struct DeleteAlign {
    allocation_meta: MemoryAllocationMeta,
    align: MemoryAlignment
}

struct DeleteArrayAlign {
    allocation_meta: MemoryAllocationMeta,
    align: MemoryAlignment
}

impl MemoryAllocationMeta {

    pub fn new(timestamp: u64, ptr: u64, key: TMtcKey) -> Self {
        MemoryAllocationMeta {timestamp, ptr: MemoryAddress{address:ptr}, key}
    }
}

trait AllocationPoint {

    fn timestamp(&self) -> u64;

    fn address(&self) -> &MemoryAddress;

    fn key(&self) -> &TMtcKey;

    fn description(&self) -> String;

}

macro_rules! generate_allocation_meta_getters {
    ($struct_name:ident) => {
        impl AllocationPoint for $struct_name {
            fn timestamp(&self) -> u64 {
                self.allocation_meta.timestamp
            }

            fn address(&self) -> &MemoryAddress {
                &self.allocation_meta.ptr
            }

            fn key(&self) -> &TMtcKey {
                &self.allocation_meta.key
            }

            fn description(&self) -> String {
                "This is a description".to_string()
            }
        }
    };
}

macro_rules! generate_new_body {
    ($struct_name:ident, $timestamp:expr, $ptr:expr, $($field_name:ident: $field_type:ty),*) => {
        $struct_name {allocation_meta: MemoryAllocationMeta::new($timestamp, $ptr, TMtcKey::$struct_name), $(
                        $field_name: {
                            if let Some(converted_value) = <$field_type as From<_>>::from($field_name).into() {
                                converted_value
                            } else {
                                $field_name
                            }
                        },
                    )*}
    };
    ($struct_name:ident, $timestamp:expr, $ptr:expr) => {
        $struct_name { allocation_meta: MemoryAllocationMeta::new($timestamp, $ptr, TMtcKey::$struct_name)}
    };
}

macro_rules! generate_tmtc_funcs {
    ($struct_name:ident, $($field_name:ident: $field_type:ty),*) => {
        impl $struct_name {
            pub fn new(timestamp: u64, ptr: u64, $($field_name: $field_type),*) -> Self {
                generate_new_body!($struct_name, timestamp, ptr, $($field_name: $field_type),*)
            }

            pub fn new_box(timestamp: u64, ptr: u64, $($field_name: $field_type),*) -> Box<Self> {
                Box::new(generate_new_body!($struct_name, timestamp, ptr, $($field_name: $field_type),*))
            }

            $(
                pub fn $field_name(&self) -> $field_type {
                    self.$field_name
                }
            )*
        }

        generate_allocation_meta_getters!($struct_name);

    };

    ($struct_name:ident) => {
        impl $struct_name {
            pub fn new(timestamp: u64, ptr: u64) -> Self {
                generate_new_body!($struct_name, timestamp, ptr)
            }

            pub fn new_box(timestamp: u64, ptr: u64) -> Box<Self> {
                Box::new(generate_new_body!($struct_name, timestamp, ptr))
            }
        }

        generate_allocation_meta_getters!($struct_name);
    };
}

generate_tmtc_funcs!(Malloc, size: usize);
generate_tmtc_funcs!(Calloc, nmemb: usize, size: usize);
generate_tmtc_funcs!(Realloc, size: usize, new_ptr: MemoryAddress);
generate_tmtc_funcs!(ReallocArray, nmemb: usize, size: usize, new_ptr: MemoryAddress);
generate_tmtc_funcs!(Free);
generate_tmtc_funcs!(New, size: usize);
generate_tmtc_funcs!(NewNothrow, size: usize);
generate_tmtc_funcs!(NewArray, size: usize);
generate_tmtc_funcs!(NewArrayNothrow, size: usize);
generate_tmtc_funcs!(Delete);
generate_tmtc_funcs!(DeleteSized, size: usize);
generate_tmtc_funcs!(DeleteNothrow);
generate_tmtc_funcs!(DeleteArray);
generate_tmtc_funcs!(DeleteArraySized, size: usize);
generate_tmtc_funcs!(DeleteArrayNothrow);
generate_tmtc_funcs!(NewAlign, size: usize, align: MemoryAlignment);
generate_tmtc_funcs!(NewArrayAlign, size: usize, align: MemoryAlignment);
generate_tmtc_funcs!(DeleteAlign, align: MemoryAlignment);
generate_tmtc_funcs!(DeleteArrayAlign, align: MemoryAlignment);



impl fmt::Display for dyn AllocationPoint {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.description().fmt(f)
    }
}

