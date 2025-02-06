

#ifndef MEMORY_MONITOR_TMTCDECODER_H
#define MEMORY_MONITOR_TMTCDECODER_H


#include <stdint.h>
#include <stdio.h>

#define T_KEY_SIZE 0x12

typedef unsigned char byte_t;

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
    char is_collapsable;
};

struct TMtcStream {
    struct TMtcObject* object;
    struct TMtcObject* _next;
    uint16_t index;
    uint64_t offset;
    FILE* fp;
    uint16_t _chunk_size;
    byte_t* _read_buffer;
    unsigned char flipper;
};

#ifdef __cplusplus
extern "C" {
#endif

void createTMtcObject(struct TMtcObject* object);

void destroyTMtcObject(struct TMtcObject* object);

uint8_t queryTDecodeProgress(struct TMtcObject* object);

void decode_tmtc(const char* filepath, struct TMtcObject* object);


void createTMtcStream(struct TMtcStream* stream);

void stream_decode_tmtc(const char* filename, struct TMtcStream* stream, char is_collapsable);

struct TMtcObject* stream_tmtc_next(const struct TMtcStream* stream);

void stream_tmtc_destroy(struct TMtcStream* stream);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_MONITOR_TMTCDECODER_H
