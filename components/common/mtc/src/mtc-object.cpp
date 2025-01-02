
#include "mtc-object.h"

namespace mtc {

    MtcObject::MtcObject(int version, std::tm timestamp): version(version), date(timestamp) {

    }

    MtcObject::MtcObject(const MtcObject& object) {
        version = object.version;
        date = object.date;
        points = object.points;
    }


    void MtcObject::add_point(const MtcPoint& point) {
        points.push_back(point);
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


}