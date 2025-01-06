
#ifndef MTC_OBJECT_H
#define MTC_OBJECT_H

#include "../../mem-monitor-config.h"
#include "mtc-point.h"
#include <chrono>
#include <map>
#include <vector>

namespace mtc {

    class MtcObject {

    private:
        int version;
        std::tm date;
        std::vector<uint16_t*> time_pointers;

        std::map<mk_size_t, std::vector<MtcPoint*>*> points_maps;
        std::map<mk_size_t, bool>* point_default;
        uint64_t length;

    public:
        MtcObject(int version, std::tm timestamp);

        void add_point(mk_size_t key, MtcPoint* point);

        int get_version() const;

        std::tm get_date() const;

        std::vector<MtcPoint*>* get_points(mk_size_t key);

        bool is_point_default(mk_size_t key);

        std::map<mk_size_t, bool>* get_default_points() const;

        uint64_t get_length() const;

        void add_time(uint16_t* time);

    };

}


#endif //MTC_OBJECT_H
