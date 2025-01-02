
#ifndef MTC_OBJECT_H
#define MTC_OBJECT_H

#include <chrono>
#include "mtc-point.h"

namespace mtc {

    class MtcObject {

    private:
        int version;
        std::tm date{};

        std::vector<MtcPoint> points;

    public:
          MtcObject(int version, std::tm timestamp);
          MtcObject(const MtcObject& object);

          void add_point(const MtcPoint& point);

          int get_version() const;

          std::tm get_date() const;

          std::vector<MtcPoint> get_points() const;
    };

}


#endif //MTC_OBJECT_H
