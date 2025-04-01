use crate::{c_createTMtcObject, mtc_point_size_t, CMtcObject, CTMtcObject};
use crate::mtcrustdecoder::TMtcObject;



pub struct TMtcObject {
    raw: CTMtcObject,
    owns_memory: bool,
}


impl TMtcObject {

    pub fn new() -> Self {
        let mut raw = CTMtcObject{
            points: std::ptr::null_mut(),
            size: 0,
            _file_length: 0,
            _allocation_size: 0,
            is_collapsable: 0
        };

        unsafe {
            c_createTMtcObject(&mut raw);
        }

        TMtcObject{
            raw,
            owns_memory: true
        }
    }
}
