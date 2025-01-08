
#ifndef MTCCDECODER_H
#define MTCCDECODER_H

#include <stdint.h>


struct MtcPoint {
    uint16_t* time_offset;
    uint16_t value;
    uint64_t repeated;
};

struct MtcPointMap {
    struct MtcPoint* points;
    uint16_t length;
};

struct MtcTime {
    uint16_t* time_offset;
    uint64_t repeated;
};

struct MtcObject {
    struct MtcPointMap* point_map;
    struct MtcTime* times;
    uint16_t length;
    uint8_t version;
    uint64_t alloc_size_points;
    uint64_t alloc_size_times;
};



struct MtcObject* decode(const char* filename);


#endif //MTCCDECODER_H
