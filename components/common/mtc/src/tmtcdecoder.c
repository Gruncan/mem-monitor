
#include "tmtcdecoder.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <string.h>
#include <unistd.h>

#define MASK_32 0xFFFFFFFF
#define MASK_8 0xFF
#define MASK_8S 0x7F

#define LOG_SIZE 34
#define MIN_LOG_SIZE 18
#define CHUNK_SIZE 10
#define INIT_SIZE 5120

#define MAX_LOG_VARS 3
#define _FILE_OFFSET_BITS 64


static uint64_t prev_micro_seconds = 0;
static char prev_key = -1;
static uint64_t prev_address = 0;


#define ARRAY_COMBINE4(array, index)                                                                                   \
    (((array)[index] << 24) | ((array)[(index) + 1] << 16) | ((array)[(index) + 2] << 8) | ((array)[(index) + 3]))


inline void createTMtcObject(struct TMtcObject* object) {
    object->_allocation_size = INIT_SIZE;
    object->points = malloc(sizeof(struct TMtcPoint) * object->_allocation_size);
    object->size = 0;
    object->_file_length = 0;
    object->is_collapsable = 1;
}

inline void destroyTMtcObject(struct TMtcObject* object) {
    if (object == NULL) {
        return;
    }

    for (uint64_t i = 0; i < object->size; i++) {
        free(object->points[i].values);
    }
    free(object->points);
}

static uint64_t getTMtcPointAddress(const struct TMtcPoint* point) {
    switch (point->key) {
        case MALLOC:
        case NEW:
        case NEW_NOTHROW:
        case NEW_ARRAY:
        case NEW_ARRAY_NOTHROW:
        case NEW_ALIGN:
        case NEW_ARRAY_ALIGN:
            return point->values[1];
        case REALLOC:
        case REALLOC_ARRAY:
        case FREE:
        case DELETE:
        case DELETE_SIZED:
        case DELETE_NOTHROW:
        case DELETE_ARRAY:
        case DELETE_ARRAY_SIZED:
        case DELETE_ARRAY_NOTHROW:
        case DELETE_ALIGN:
        case DELETE_ARRAY_ALIGN:
            return point->values[0];
        case CALLOC:
            return point->values[2];
        default:
            return 0;
    }
}

static unsigned char isTMtcKeyEncapsulated(const uint8_t new_key, const uint8_t old_key) {
    switch (old_key) {
        case MALLOC:
            return new_key == NEW;
        case NEW:
            return new_key == NEW_ARRAY;
        case FREE:
            return new_key == DELETE;
        case DELETE:
            return new_key == DELETE_ARRAY || new_key == DELETE_SIZED || new_key == DELETE_NOTHROW;
        default:
            return 0;
    }
}


static unsigned char shouldTMtcPointOverride(const struct TMtcPoint* point) {
    const uint64_t address = getTMtcPointAddress(point);

    if (prev_key == -1) {
        goto setPrevKeyAddress;
    }

    if (isTMtcKeyEncapsulated(point->key, prev_key) && prev_address == address) {
        prev_key = (char) (point->key & MASK_8S);
        prev_address = address;
        return 1;
    }

setPrevKeyAddress:
    prev_key = (char) (point->key & MASK_8S);
    prev_address = address;
    return 0;
}


inline uint8_t queryTDecodeProgress(struct TMtcObject* object) {
    if (object == NULL || object->_file_length == 0 || object->size == 0) {
        return 0;
    }
    return (uint8_t) (((double) (object->size * LOG_SIZE) / (double) object->_file_length) * 100) & MASK_8;
}

static uint8_t decode_tchunk(const byte_t* buffer, struct TMtcObject* object) {
    uint64_t seconds = ARRAY_COMBINE4(buffer, 0);
    uint64_t micro_seconds = ARRAY_COMBINE4(buffer, 4);
    micro_seconds += seconds * 1000000;

    if (object->size == object->_allocation_size) {
        object->_allocation_size *= 2;
        void* new_ptr = realloc(object->points, object->_allocation_size * sizeof(struct TMtcPoint));
        if (new_ptr == NULL) {
            perror("Failed to realloc points TMTC!");
            _exit(-1);
        }
        object->points = new_ptr;
    }


    struct TMtcPoint* point = &object->points[object->size];
    point->key = buffer[8];
    const uint8_t length = buffer[9];
    point->length = length;
    point->values = malloc(length * sizeof(uint64_t));
    if (point->values == NULL) {
        perror("Failed to malloc points TMTC!");
        _exit(-1);
    }

    memset(point->values, 0, length * sizeof(uint64_t));

    uint64_t* values = point->values;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 8; ++j) {
            values[i] |= (uint64_t) buffer[(i * 8) + 10 + j] << (8 * (7 - j));
        }
    }

    if (shouldTMtcPointOverride(point) && object->is_collapsable && object->size > 0) {
        free(object->points[object->size - 1].values);
        uint32_t timeoffset = object->points[object->size - 1].time_offset;
        point->time_offset = timeoffset;
        object->points[object->size - 1] = *point;
        goto exitFunction;
    }

    if (object->size == 0) {
        point->time_offset = prev_micro_seconds;
    } else {
        if (micro_seconds < prev_micro_seconds) {
            point->time_offset = 0;
        } else {
            point->time_offset = (micro_seconds - prev_micro_seconds) & MASK_32;
        }
    }

    prev_micro_seconds = micro_seconds;

    object->size++;

exitFunction:
    return (MAX_LOG_VARS - length) * sizeof(uint64_t);
}

int has_extension(const char* filename, const char* extension) {
    const char* dot = strrchr(filename, '.');
    return (dot && strcmp(dot + 1, extension) == 0);
}


static char parse_tmtc_file(byte_t* buffer, const uint16_t internalChunk, FILE* fp, struct TMtcObject* object) {
    size_t bytesRead = 0;

    bytesRead = fread(buffer, 1, internalChunk, fp);
    if (bytesRead < MIN_LOG_SIZE) {
        return 1;
    }

    uint16_t offset = 0;

    while (bytesRead >= offset + MIN_LOG_SIZE) {
        const uint8_t overshot = decode_tchunk(buffer + offset, object);
        offset += LOG_SIZE - overshot;
    }

    if (fseeko(fp, -((off_t) bytesRead - offset), SEEK_CUR) != 0) {
        perror("Failed to shift overshot offset");
        return -1;
    }

    return 0;
}


void decode_tmtc(const char* filename, struct TMtcObject* object) {
    if (!has_extension(filename, "tmtc")) {
        fprintf(stderr, "This decoder only supports tmtc extensions!\n");
    }

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open file!");
        return;
    }

    // TODO make this dynamic based on file length
    static const uint16_t CHUNKING_SIZE = LOG_SIZE * CHUNK_SIZE;

    byte_t* buffer = malloc(CHUNKING_SIZE);
    if (buffer == NULL) {
        perror("Failed to allocate buffer!");
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_END);
#ifdef __unix__
    object->_file_length = ftell(fp);
#else
    object->_file_length = _ftelli64(fp);
#endif
    fseek(fp, 0, SEEK_SET);

    size_t bytesRead = 0;

    while ((bytesRead = fread(buffer, 1, CHUNKING_SIZE, fp)) > 0) {
        if (bytesRead < MIN_LOG_SIZE) {
            break;
        }

        uint16_t offset = 0;

        while (bytesRead >= offset + MIN_LOG_SIZE) {
            const uint8_t overshot = decode_tchunk(buffer + offset, object);
            offset += LOG_SIZE - overshot;
        }

        if (fseeko(fp, -((off_t) bytesRead - offset), SEEK_CUR) != 0) {
            perror("Failed to shift overshot offset");
            goto cleanUpFunction;
        }
    }

    void* new_points_ptr = realloc(object->points, object->size * sizeof(struct TMtcPoint));
    if (new_points_ptr == NULL) {
        perror("Failed to shrink point buffer!");
        goto cleanUpFunction;
    }
    object->points = new_points_ptr;

cleanUpFunction:
    free(buffer);
    fclose(fp);
}


void createTMtcStream(struct TMtcStream* stream) {
    stream->object = malloc(sizeof(struct TMtcObject) * 2); // More efficient than 1 mallocs
    stream->_next = stream->object + 1;

    if (stream->object == NULL || stream->_next == NULL) {
        perror("Failed to allocate stream object!");
        _exit(-1);
    }

    createTMtcObject(stream->object);
    createTMtcObject(stream->object);

    stream->index = 0;
    stream->offset = 0;

    stream->fp = NULL;
    stream->_chunk_size = 0;
    stream->flipper = 0;
}


void stream_decode_tmtc(const char* filename, struct TMtcStream* stream, const char is_collapsable) {
    if (stream->fp != NULL) {
        fprintf(stderr, "Stream of the file has already been opened!\n");
        return;
    }

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open file!");
        return;
    }
    stream->fp = fp;

    stream->_chunk_size = LOG_SIZE * CHUNK_SIZE;
    stream->_read_buffer = malloc(stream->_chunk_size);
    stream->object->is_collapsable = is_collapsable;

    fseek(fp, 0, SEEK_END);
#ifdef __unix__
    stream->object->_file_length = ftell(fp);
#else
    stream->object->_file_length = _ftelli64(fp);
#endif
    fseek(fp, 0, SEEK_SET);

    stream->_next->_file_length = stream->object->_file_length;
}


struct TMtcObject* stream_tmtc_next(const struct TMtcStream* stream) {
    stream->object->size = 0;
    if (parse_tmtc_file(stream->_read_buffer, stream->_chunk_size, stream->fp, stream->object) != 0) {
        return NULL;
    }

    return stream->object;
}

void stream_tmtc_destroy(struct TMtcStream* stream) {
}
