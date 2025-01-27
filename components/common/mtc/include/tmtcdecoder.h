

#ifndef MEMORY_MONITOR_TMTCDECODER_H
#define MEMORY_MONITOR_TMTCDECODER_H


#include <stdint.h>

#define T_KEY_SIZE 0x12

#define MALLOC 0x0
#define CALLOC 0x1
#define REALLOC 0x2
#define REALLOC_ARRAY 0x3
#define FREE 0x4

#define NEW 0x5
#define NEW_NOTHROW 0x6
#define NEW_ARRAY 0x7
#define NEW_ARRAY_NOTHROW 0x8

#define DELETE 0x9
#define DELETE_SIZED 0xa
#define DELETE_NOTHROW 0xb
#define DELETE_ARRAY 0xc
#define DELETE_ARRAY_SIZED 0xd
#define DELETE_ARRAY_NOTHROW 0xe

#define NEW_ALIGN 0xf
#define NEW_ARRAY_ALIGN 0x10
#define DELETE_ALIGN 0x11
#define DELETE_ARRAY_ALIGN 0x12


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

void initaliseTMtcObject(struct TMtcObject* object);

uint8_t queryTDecodeProgress(struct TMtcObject* object);

void decode_tmtc(const char* filepath, struct TMtcObject* object);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_MONITOR_TMTCDECODER_H
