use crate::bindings::*;



pub struct TMtcObjectFfi {
    raw: TMtcObject,
    values: Vec<TMtcPointFfi>,
    owns_memory: bool,
}

pub struct TMtcPointFfi {
    raw: TMtcPoint,
}


impl TMtcObjectFfi {

    pub fn new() -> Self {
        let mut raw = TMtcObject{
            points: std::ptr::null_mut(),
            size: 0,
            _file_length: 0,
            _allocation_size: 0,
            is_collapsable: 0
        };

        unsafe {
            createTMtcObject(&mut raw);
        }

        TMtcObjectFfi{
            raw,
            owns_memory: true,
            values: unsafe {
                let mut vec = Vec::with_capacity(raw.size as usize);
                for i in 0..raw.size {
                    vec.push(TMtcPointFfi {raw: *raw.points.wrapping_add(i as usize) });
                }
                vec
            }
        }
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

    pub fn get_points(&self) -> &Vec<TMtcPointFfi> {
        &self.values
    }

}

impl Drop for TMtcObjectFfi {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                destroyTMtcObject(&mut self.raw);
            }
        }
    }
}