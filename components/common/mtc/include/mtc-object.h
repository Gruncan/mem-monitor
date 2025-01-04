
#ifndef MTC_OBJECT_H
#define MTC_OBJECT_H

#include <chrono>
#include "mtc-point.h"

namespace mtc {

    class MtcObject {

    private:
        int version;
        std::tm date{};
        std::vector<uint16_t*> time_pointers;

        std::map<uint8_t, std::vector<MtcPoint*>*> points_maps;
        uint64_t length;

    public:
        MtcObject(int version, std::tm timestamp);

        void add_point(uint8_t key, MtcPoint* point);

        int get_version() const;

        std::tm get_date() const;

        std::vector<MtcPoint*>* get_points(uint8_t key);

        uint64_t get_length() const;

        void add_time(uint16_t* time);

    };

}


#endif //MTC_OBJECT_H
