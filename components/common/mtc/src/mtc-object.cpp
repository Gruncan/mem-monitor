
#include "mtc-object.h"

namespace mtc {

    MtcObject::MtcObject(int version, std::tm timestamp): version(version), date(timestamp), length(0) {

    }

    inline void MtcObject::add_point(const uint8_t key, MtcPoint* point) {
        if (!points_maps.contains(key)) {
            points_maps[key] = new std::vector<MtcPoint*>(length + 1);
        }
        points_maps[key]->push_back(point);
    }

    inline int MtcObject::get_version() const {
        return version;
    }

    inline std::tm MtcObject::get_date() const {
        return date;
    }

    inline std::vector<MtcPoint*>* MtcObject::get_points(const uint8_t key){
        return points_maps[key];
    }

    inline uint64_t MtcObject::get_length() const {
        return length;
    }

    inline void MtcObject::add_time(uint16_t* time) {
        time_pointers.push_back(time);
        length++;
    }


}