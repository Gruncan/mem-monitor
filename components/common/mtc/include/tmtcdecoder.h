

#ifndef MEMORY_MONITOR_TMTCDECODER_H
#define MEMORY_MONITOR_TMTCDECODER_H


#include <stdint.h>


#define T_KEY_SIZE 0x12


struct TMtcPoint {
    uint8_t key;
    uint8_t length;
    uint32_t time_offset;
    uint64_t* values;
};


struct TMtcObject {
    struct TMtcPoint* points;
    uint64_t size;
    uint64_t _file_length;
    uint64_t _allocation_size;
};

#ifdef __cplusplus
extern "C" {
#endif

void createTMtcObject(struct TMtcObject* object);

void destroyTMtcObject(struct TMtcObject* object);

uint8_t queryTDecodeProgress(struct TMtcObject* object);

void decode_tmtc(const char* filepath, struct TMtcObject* object);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_MONITOR_TMTCDECODER_H
