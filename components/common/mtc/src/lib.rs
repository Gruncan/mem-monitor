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
            MtcError::InitialisationFailure(ref message) => write!(f, "Failed to initialise (T)MTC object! Reason: {}", message),
            MtcError::DataReadFailure(ref message) => write!(f, "Failed to read (T)MTC object data! Reason: {}", message),
        }
    }
}

impl MtcError {
    pub fn initialisation_failure(message: String) -> MtcError {
        Self::InitialisationFailure(message)
    }

    pub fn non_constructed(message: String) -> MtcError {
        Self::NonConstructed(message)
    }

    pub fn decode_failure(message: String) -> MtcError {
        Self::DecodeFailure(message)
    }

    pub fn data_read_failure(message: String) -> MtcError {
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