
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
        uint64_t length;

    public:
          MtcObject(int version, std::tm timestamp);
          MtcObject(const MtcObject& object);

          void add_point(const MtcPoint& point);

          int get_version() const;

          std::tm get_date() const;

          std::vector<MtcPoint> get_points() const;

             uint64_t get_length() const;
    };

}


#endif //MTC_OBJECT_H
