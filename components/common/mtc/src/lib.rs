mod bindings;
mod mtcrustdecoder;
mod tmtcrustdecoder;

use std::fmt;
pub use mtcrustdecoder::*;
pub use tmtcrustdecoder::*;


#[derive(Debug)]
pub enum MtcError {
    InitialisationFailure(String),
    NonConstructed(String),
    DecodeFailure(String),
    DataReadFailure(String),
}

impl fmt::Display for MtcError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            MtcError::DecodeFailure(ref message) =>
                write!(f, "Failed to decode MTC! Reason: {}", message),
            MtcError::NonConstructed(ref message) => write!(f, "{}", message),
            MtcError::InitialisationFailure(ref message) => write!(f, "Failed to initialise MTC object! Reason: {}", message),
            MtcError::DataReadFailure(ref message) => write!(f, "Failed to read MTC object data! Reason: {}", message),
        }
    }
}

impl MtcError {
    pub fn initialisation_failure(message: String) -> Self {
        Self::InitialisationFailure(message)
    }

    pub fn non_constructed(message: String) -> Self {
        Self::NonConstructed(message)
    }

    pub fn decode_failure(message: String) -> Self {
        Self::DecodeFailure(message)
    }

    pub fn data_read_failure(message: String) -> Self {
        Self::DataReadFailure(message)
    }
}


impl std::error::Error for MtcError {

}

#[macro_export]
macro_rules! handle_mtc_result {
    ($result:expr, $success_value:expr, $error_func:ident) => {
        match $result {
            Ok(_) => Ok($success_value),
            Err(payload) => {
                match payload.downcast::<String>() {
                    Ok(message) => Err(MtcError::$error_func(*message)),
                    Err(_) => Err(MtcError::$error_func("Unknown".to_string()))
                }
            }
        }
    };
}

#[macro_export]
macro_rules! get_mtc_struct_from_ptr {
    ($ptr:expr, $msg:expr) => {{
        fn __enforce_ptr<T>(ptr: *const T) -> *const T {
            ptr
        }
        let raw_ptr = __enforce_ptr($ptr);
        if raw_ptr.is_null() {
            panic!($msg);
        }
        *raw_ptr
    }};

    ($ptr:expr) => {
        get_mtc_struct_from_ptr!($ptr, "Ptr is null.")
    };
}