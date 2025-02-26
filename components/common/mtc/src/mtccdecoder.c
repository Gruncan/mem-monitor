
#include "mtccdecoder.h"

#include "mem-monitor-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HEADER_SIZE 5
#define INIT_SIZE 5120

#define KEY_SIZE_PROC 225
#define KEY_SIZE_NO_PROC 216



#define MAX_PROC_SIZE(version) ((KEY_SIZE_PROC * MTC_WRITE_OFFSET[(version)-1]) + 4)
#define MAX_NO_PROC_SIZE(version) ((KEY_SIZE_NO_PROC * MTC_WRITE_OFFSET[(version)-1]) + 4)

static uint16_t CHUNK_SIZE = 0;

static mk_size_t KEY_SIZE = KEY_SIZE_PROC;

#define BUFFER_CHECK(buffer) \
    if ((buffer) == NULL) {   \
        perror("Failed to allocate buffer!");   \
        fclose(fp);     \
        return;     \
    } \


GEN_MTC_LOAD_FUNC_IMP(1, 2)

GEN_MTC_LOAD_FUNC_IMP(3, 3)

GEN_MTC_LOAD_FUNC_IMP(5, 4)

static mtc_point_size_t load_mtc_data(const uint8_t version, const byte_t* buffer, const uint16_t index) {
    switch (version) {
        case 1:
        case 2:
            return MTC_LOAD_DATA(1, buffer, index);
        case 3:
        case 4:
            return MTC_LOAD_DATA(3, buffer, index);
        case 5:
        case 6:
            return MTC_LOAD_DATA(5, buffer, index);
        default:
            return 0;
    }
}


inline void createMtcObject(struct MtcObject* object) {
    object->point_map = malloc(sizeof(struct MtcPointMap) * KEY_SIZE);
    object->_alloc_size_points = INIT_SIZE;
    for (mk_size_t i = 0; i < KEY_SIZE; i++) {
        object->point_map[i].points = malloc(sizeof(struct MtcPoint) * object->_alloc_size_points);
        object->point_map[i].points[0].repeated = 0;
        object->point_map[i].points[0].time_offset = malloc(sizeof(uint16_t));
        object->point_map[i].length = 0;
    }
    object->_alloc_size_times = INIT_SIZE;
    object->times = malloc(sizeof(struct MtcTime) * object->_alloc_size_times);
    object->times[0].repeated = 0;
    object->times[0].time_offset = malloc(sizeof(uint16_t));
    object->_times_length = 0;
    object->version = 0;
    object->file_length = 0;
    object->size = 0;
    object->_key_size = KEY_SIZE;
}

// TODO implement a destroy function

static void decode_header(const byte_t* buffer, struct MtcObject* object) {
    object->version = buffer[0];
    // todo add time decoding here
}

inline uint8_t queryDecodeProgress(struct MtcObject* object) {
    if (object->file_length == 0 || object->size == 0) {
        return 0;
    }
    const uint8_t value = ((double) (object->size * CHUNK_SIZE) / (double) object->file_length) * 100;
    return value;
}

static void decode_chunk(const byte_t* buffer, struct MtcObject* object) {
    if (object == NULL) {
        return;
    }

    const uint16_t time_offset = buffer[0] << 8 | buffer[1];
    if (object->_times_length == object->_alloc_size_times) {
        object->_alloc_size_times *= 2;
        void* new_ptr = realloc(object->times, object->_alloc_size_times * sizeof(struct MtcTime));
        if (new_ptr == NULL) {
            perror("Failed to realloc MTC point times!");
            exit(-1);
        }
        object->times = new_ptr;
    }
    if (object->_times_length == 0) {
        *object->times[0].time_offset = time_offset;
        object->_times_length++;
    } else {
        if (*object->times[object->_times_length - 1].time_offset == time_offset) {
            object->times[object->_times_length - 1].repeated++;
        } else {
            object->times[object->_times_length].time_offset = malloc(sizeof(uint16_t));
            *object->times[object->_times_length].time_offset = time_offset;

            object->times[object->_times_length].repeated = 0;
            object->_times_length++;
        }
    }

    const uint8_t WRITE_OFFSET = MTC_WRITE_OFFSET[object->version-1];
    const uint16_t length_offset = buffer[2] << 8 | buffer[3];
    for (uint16_t i = 4; i < length_offset + 4; i += WRITE_OFFSET) {
        const mk_size_t key = buffer[i];
        if (key >= KEY_SIZE) {
            fprintf(stderr, "Key size out of range\nThis is likely due to version encoding/decoding mismatch!");
            exit(-1);
        }

        const mtc_point_size_t value = load_mtc_data(object->version, buffer, i);
        if (object->point_map[key].length == object->_alloc_size_points) {
            object->_alloc_size_points *= 2;
            for (mk_size_t j = 0; j < KEY_SIZE; j++) {
                void* new_ptr =
                    realloc(object->point_map[j].points, object->_alloc_size_points * sizeof(struct MtcPoint));
                if (new_ptr == NULL) {
                    perror("Failed to realloc MTC point map!");
                    exit(-1);
                }
                object->point_map[j].points = new_ptr;
            }
        }

        if (object->point_map[key].length == 0) {
            object->point_map[key].points[0].time_offset = object->times[object->_times_length - 1].time_offset;
            object->point_map[key].points[0].value = value;
            object->point_map[key].points[0].repeated = 0;
            object->point_map[key].length++;
        } else {
            const uint64_t length = object->point_map[key].length;
            if (object->point_map[key].points[length - 1].value == value) {
                object->point_map[key].points[length - 1].repeated++;
            } else {
                object->point_map[key].points[length].time_offset =
                    object->times[object->_times_length - 1].time_offset;
                object->point_map[key].points[length].value = value;
                object->point_map[key].points[length].repeated = 0;
                object->point_map[key].length++;
            }
        }
    }
}

static int has_extension(const char* filename, const char* extension) {
    const char* dot = strrchr(filename, '.');
    return (dot && strcmp(dot + 1, extension) == 0);
}


void decode(const char* filename, struct MtcObject* object) {
    if (!has_extension(filename, "mtc")) {
        fprintf(stderr, "This decoder only supports mtc extensions!\n");
    }

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open file!");
        return;
    }

    byte_t* header_buffer = malloc(HEADER_SIZE * 2);
    BUFFER_CHECK(header_buffer);

    fseek(fp, 0, SEEK_END);
#ifdef __unix__
    object->file_length = ftell(fp);
#else
    object->file_length = _ftelli64(fp);
#endif
    fseek(fp, 0, SEEK_SET);

    size_t bytesRead = 0;
    bytesRead = fread(header_buffer, 1, HEADER_SIZE, fp);

    if (bytesRead != HEADER_SIZE) {
        perror("Failed to read header!");
        goto cleanUpFunction;
    }

    decode_header(header_buffer, object);

    free(header_buffer);

    if (object->version % 2 != 0) {
        CHUNK_SIZE = MAX_NO_PROC_SIZE(object->version);
        // TODO fix this so redundant memory is wasted, we still alloc memory for all keys but not used.
        KEY_SIZE = KEY_SIZE_NO_PROC;
        object->_key_size = KEY_SIZE_NO_PROC;
    }else {
        CHUNK_SIZE = MAX_PROC_SIZE(object->version);
    }

    byte_t* buffer = malloc(CHUNK_SIZE);
    BUFFER_CHECK(buffer);

    while ((bytesRead = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
        if (bytesRead != CHUNK_SIZE) {
            // if we don't read full amount we assume corruption, write being killed part way through
            break;
        }
        decode_chunk(buffer, object);
        object->size++;
    }

    void* new_times_ptr = realloc(object->times, object->_times_length * sizeof(struct MtcTime));
    if (new_times_ptr == NULL) {
        perror("Failed to realloc point times");
        goto cleanUpFunction;
    }
    object->times = new_times_ptr;

    for (mk_size_t i = 0; i < KEY_SIZE; i++) {
        void* new_points_ptr =
            realloc(object->point_map[i].points, object->point_map[i].length * sizeof(struct MtcPoint));
        if (new_points_ptr == NULL) {
            perror("Failed to realloc point map");
            goto cleanUpFunction;
        }
        object->point_map[i].points = new_points_ptr;
    }

cleanUpFunction:
    free(buffer);
    fclose(fp);
}
