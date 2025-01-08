
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
    uint64_t size;
    uint8_t version;
    uint16_t _times_length;
    uint64_t _alloc_size_points;
    uint64_t _alloc_size_times;
};



#ifdef __cplusplus
extern "C" {
#endif
struct MtcObject* decode(const char* filename);
#ifdef __cplusplus
}
#endif

#endif //MTCCDECODER_H
