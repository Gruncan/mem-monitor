
#include "mtc-object.h"

namespace mtc {

    MtcObject::MtcObject(int version, std::tm timestamp): version(version), date(timestamp), length(0) {

    }

    MtcObject::MtcObject(const MtcObject& object) {
        version = object.version;
        date = object.date;
        points = object.points;
        length = object.length;
    }


    void MtcObject::add_point(const MtcPoint& point) {
        points.push_back(point);
        length++;
    }

    int MtcObject::get_version() const {
        return version;
    }

    std::tm MtcObject::get_date() const {
        return date;
    }

    std::vector<MtcPoint> MtcObject::get_points() const {
        return points;
    }

    uint64_t MtcObject::get_length() const {
        return length;
    }


}