
#include "mtccdecoder.h"

#include "../../mem-monitor-config.h"

#include <stdio.h>
#include <stdlib.h>

#define HEADER_SIZE 5
#define INIT_SIZE 5120

const static uint16_t CHUNK_SIZE = 679;

typedef unsigned char byte;

inline void initaliseMtcObject(struct MtcObject* object) {
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
}

inline static void decode_header(const byte* buffer, struct MtcObject* object) {
    object->version = buffer[0];
    //todo add time decoding here
}

inline uint8_t queryDecodeProgress(struct MtcObject* object) {
    if (object->file_length == 0 || object->size == 0) {
        return 0;
    }
    const uint8_t value = ((double)(object->size * CHUNK_SIZE) / (double) object->file_length) * 100;
    return value;
}

static void decode_chunk(const byte* buffer, struct MtcObject* object) {
    if (object == NULL) {
        return;
    }


    const uint16_t time_offset = buffer[0] << 8 | buffer[1];
    if (object->_times_length == object->_alloc_size_times) {
        object->_alloc_size_times *= 2;
        void* new_ptr = realloc(object->times, object->_alloc_size_times * sizeof(struct MtcTime));
        if (new_ptr == NULL) {
            perror("Failed to realloc point times");
        }
        object->times = new_ptr;
    }
    if (object->_times_length == 0) {
        *object->times[0].time_offset = time_offset;
        object->_times_length++;
    } else {
        if (*object->times[object->_times_length-1].time_offset == time_offset) {
            object->times[object->_times_length-1].repeated++;
        } else {
            object->times[object->_times_length].time_offset = malloc(sizeof(uint16_t));
            *object->times[object->_times_length].time_offset = time_offset;

            object->times[object->_times_length].repeated = 0;
            object->_times_length++;
        }
    }
    const uint16_t length_offset = buffer[2] << 8 | buffer[3];
    for (uint16_t i = 4; i < length_offset + 4; i += 3) {
        const mk_size_t key = buffer[i];
        const uint16_t value = buffer[i + 1] << 8 | buffer[i + 2];
        if (object->point_map[key].length == object->_alloc_size_points) {
            object->_alloc_size_points *= 2;
            for (mk_size_t j = 0; j < KEY_SIZE; j++) {
                void* new_ptr =  realloc(object->point_map[j].points, object->_alloc_size_points * sizeof(struct MtcPoint));
                if (new_ptr == NULL) {
                    perror("Failed to realloc point map");
                    return;
                }
                object->point_map[j].points = new_ptr;
            }

        }

        if (object->point_map[key].length == 0) {
            object->point_map[key].points[0].time_offset = object->times[object->_times_length-1].time_offset;
            object->point_map[key].points[0].value = value;
            object->point_map[key].points[0].repeated = 0;
            object->point_map[key].length++;
        } else {
            const uint64_t length = object->point_map[key].length;
            if (object->point_map[key].points[length-1].value == value) {
                object->point_map[key].points[length-1].repeated++;
            }else {
                object->point_map[key].points[length].time_offset = object->times[object->_times_length-1].time_offset;
                object->point_map[key].points[length].value = value;
                object->point_map[key].points[length].repeated = 0;
                object->point_map[key].length++;
            }
        }
    }
}


void decode(const char* filename, struct MtcObject* object) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open file!");
        return;
    }

    byte* buffer = malloc(CHUNK_SIZE);
    if (buffer == NULL) {
        perror("Failed to allocate buffer!");
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_END);
    object->file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    size_t bytesRead = 0;
    bytesRead = fread(buffer, 1, HEADER_SIZE, fp);

    if (bytesRead != HEADER_SIZE) {
        perror("Failed to read header!");
        fclose(fp);
    }

    decode_header(buffer, object);



    while ((bytesRead = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
        if (bytesRead != CHUNK_SIZE) {
            break;
        }
        decode_chunk(buffer, object);
        object->size++;
    }

    void* new_times_ptr = realloc(object->times, object->_times_length * sizeof(struct MtcTime));
    if (new_times_ptr == NULL) {
        perror("Failed to realloc point times");
        return;
    }
    object->times = new_times_ptr;

    for (mk_size_t i = 0; i < KEY_SIZE; i++) {
        void* new_points_ptr =  realloc(object->point_map[i].points, object->point_map[i].length * sizeof(struct MtcPoint));
        if (new_points_ptr == NULL) {
            perror("Failed to realloc point map");
            return;
        }
        object->point_map[i].points = new_points_ptr;
    }


    free(buffer);
    fclose(fp);
}
