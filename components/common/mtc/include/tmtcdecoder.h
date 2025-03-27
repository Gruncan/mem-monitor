/***********************************
 *
 * @brief The TMTC C decoder signatures
 * @details Limited support for windows, functionality is undetermined
 *
 * @file tmtccdecoder.h
 *
************************************/

#ifndef MEMORY_MONITOR_TMTCDECODER_H
#define MEMORY_MONITOR_TMTCDECODER_H


#include "mem-monitor-config.h"
#include <stdint.h>
#include <stdio.h>


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

/**
 * Initialise a TMtcObject
 * @param object the pointer to be initialised
 */
void createTMtcObject(struct TMtcObject* object);

/**
 * Destroy a TMtcObject
 * @param object the pointer to be destroyed
 */
void destroyTMtcObject(struct TMtcObject* object);

/**
 * Query the decode progress
 * @param object the decode progress object to be queried
 * @return an int from 0-100 for percentage decoded
 */
uint8_t queryTDecodeProgress(struct TMtcObject* object);

/**
 * Decode a TMTC file
 * @param filename the file to decode
 * @param object the object to be populated
 */
void decode_tmtc(const char* filepath, struct TMtcObject* object);

/**
 * Initialise a TMtcStream
 * @param stream the pointer to be initialised
 */
void createTMtcStream(struct TMtcStream* stream);

/**
 * Initialise a stream of a file
 * @param filename the file initialise a stream
 * @param stream the meta object of the stream
 * @param is_collapsable should the data be collapsed
 */
void stream_decode_tmtc(const char* filename, struct TMtcStream* stream, char is_collapsable);

/**
 * Stream the next chunk of data
 * @param stream the stream meta object
 * @return The returned decoded chunk data pointer
 */
struct TMtcObject* stream_tmtc_next(const struct TMtcStream* stream);

/**
 * Destroy a tmtc stream object
 * @param stream
 */
void stream_tmtc_destroy(struct TMtcStream* stream);

#ifdef __cplusplus
}
#endif

#endif // MEMORY_MONITOR_TMTCDECODER_H
