
#include "mtc-object.h"

namespace mtc {

    MtcObject::MtcObject(int version, std::tm timestamp): version(version), date(timestamp), length(0) {
        point_default = new std::map<uint8_t, bool>();
    }

    void MtcObject::add_point(const uint8_t key, MtcPoint* point) {
        if (!points_maps.contains(key)) {
            points_maps[key] = new std::vector<MtcPoint*>();
            (*point_default)[key] = true;
        }
        if (point->value != 0) {
            (*point_default)[key] = false;
        }
        points_maps[key]->push_back(point);
    }

    int MtcObject::get_version() const {
        return version;
    }

    std::tm MtcObject::get_date() const {
        return date;
    }

    std::vector<MtcPoint*>* MtcObject::get_points(const uint8_t key){
        return points_maps[key];
    }

    bool MtcObject::is_point_default(const uint8_t key) {
        return point_default->at(key);
    }

    std::map<uint8_t, bool>* MtcObject::get_default_points() const {
        return point_default;
    }

    uint64_t MtcObject::get_length() const {
        return length;
    }

    void MtcObject::add_time(uint16_t* time) {
        time_pointers.push_back(time);
        length++;
    }




}