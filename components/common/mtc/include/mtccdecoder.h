
#ifndef MTCCDECODER_H
#define MTCCDECODER_H


#define MTC_LOAD_FUNC_NAME(version) load_mtc_##version##_value_data

#define MTC_LOAD_DATA(version, buffer, index) MTC_LOAD_FUNC_NAME(version)(buffer, index)

#define GEN_MTC_LOAD_FUNC_SIG(version)                                                                                 \
    mtc_point_size_t MTC_LOAD_FUNC_NAME(version)(const byte_t* buffer, uint16_t index)

#define GEN_MTC_LOAD_FUNC_IMP(version, count)                                                                          \
    inline GEN_MTC_LOAD_FUNC_SIG(version) {                                                                            \
        mtc_point_size_t value = 0;                                                                                    \
        for (uint8_t i = 1; i <= (count); i++) {                                                                       \
            value |= (mtc_point_size_t) (buffer)[(index) + i] << (((count) - i) * 8);                                  \
        }                                                                                                              \
        return value;                                                                                                  \
    }

#include "stdint.h"

#include "mem-monitor-config.h"

struct MtcPoint {
    uint16_t* time_offset;
    mtc_point_size_t value;
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
    mk_size_t _key_size;
};


#ifdef __cplusplus
extern "C" {
#endif

void decode(const char* filename, struct MtcObject* object);

void createMtcObject(struct MtcObject* object);

uint8_t queryDecodeProgress(struct MtcObject* object);


GEN_MTC_LOAD_FUNC_SIG(1);

GEN_MTC_LOAD_FUNC_SIG(3);

GEN_MTC_LOAD_FUNC_SIG(5);

#ifdef __cplusplus
}
#endif

#endif // MTCCDECODER_H
