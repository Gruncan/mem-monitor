
#ifndef MTCCDECODER_H
#define MTCCDECODER_H


#define KEY_SIZE 225

#include "stdint.h"

#include <bits/pthreadtypes.h>

struct MtcPoint {
    uint16_t* time_offset;
    uint16_t value;
    uint64_t repeated;
};

struct MtcPointMap {
    struct MtcPoint* points;
    uint64_t length;
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
    uint64_t _times_length;
    uint64_t _alloc_size_points;
    uint64_t _alloc_size_times;
    uint64_t file_length;

};



#ifdef __cplusplus
extern "C" {
#endif
void decode(const char* filename, struct MtcObject* object);

void initaliseMtcObject(struct MtcObject* object);

uint8_t queryDecodeProgress(struct MtcObject* object);
#ifdef __cplusplus
}
#endif

#endif //MTCCDECODER_H
