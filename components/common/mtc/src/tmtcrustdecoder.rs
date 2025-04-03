use crate::{c_createTMtcObject, c_destroyTMtcObject, mtc_point_size_t, CMtcObject, CTMtcObject, CTMtcPoint};
use crate::mtcrustdecoder::TMtcObject;



pub struct TMtcObject {
    raw: CTMtcObject,
    values: Vec<TMtcPoint>,
    owns_memory: bool,
}

pub struct TMtcPoint {
    raw: CTMtcPoint,
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
            owns_memory: true,
            values: {
                let mut vec = Vec::with_capacity(raw.size as usize);
                for i in 0..raw.size {
                    vec.push(TMtcPoint {raw: &mut raw.points[i as usize]});
                }
                vec
            }
        }
    }

    pub fn get_size(&self) -> usize {
        self.raw.size as usize
    }

    pub fn get_is_collapsable(&self) -> bool {
        if self.raw.is_collapsable {
            true
        } else {
            false
        }
    }

    pub fn get_points(&self) -> Vec<CTMtcPoint> {
        for i in 0..self.get_size() {
            TMtcPoint {raw: self.raw.points[i as usize]}
        }
    }

}

impl Drop for TMtcObject {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                c_destroyTMtcObject(&mut self.raw);
            }
        }
    }
}