
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "tmtcdecoder.h"

#define MASK_32 0xFFFFFFFF
#define LOG_SIZE 13
#define CHUNK_SIZE 1
#define INIT_SIZE 5120

#define MAX_LOG_VARS 3

typedef unsigned char byte;

static uint64_t prev_micro_seconds = 0;

#define ARRAY_COMBINE4(array, index) (((array)[index] << 24) | ((array)[(index) + 1] << 16) | ((array)[(index) + 2] << 8) | ((array)[(index) + 3]))


inline void initaliseTMtcObject(struct TMtcObject* object) {
    object->_allocation_size = INIT_SIZE;
    object->points = malloc(sizeof(struct TMtcPoint) * object->_allocation_size);
    object->size = 0;
    object->_file_length = 0;
}

inline uint8_t queryTDecodeProgress(struct TMtcObject* object) {
    return 0;
}

static uint8_t decode_tchunk(const byte* buffer, struct TMtcObject* object){
    uint64_t micro_seconds = (ARRAY_COMBINE4(buffer, 0) * 1000000) + ARRAY_COMBINE4(buffer, 4);
    if (object->size == object->_allocation_size){
        object->_allocation_size *= 2;
        void* new_ptr = realloc(object->points, object->_allocation_size * sizeof(struct TMtcPoint));
        if (new_ptr == NULL){
            perror("Failed to realloc points TMTC!");
            exit(-1);
        }
        object->points = new_ptr;
    }

    struct TMtcPoint* point = &object->points[object->size];
    point->key = buffer[8];
    uint8_t length = buffer[9];
    point->length = length;
    point->values = calloc(0, length);
    uint64_t* values = point->values;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 8; ++j) {
            values[i] |= buffer[(i * 8) + 10 + j] << (8 * (7 - j));
        }
    }

    if (object->size == 0){
        point->time_offset = prev_micro_seconds;
    }else{
        point->time_offset = (micro_seconds - prev_micro_seconds) & MASK_32;
    }

    prev_micro_seconds = micro_seconds;

    object->size++;

    return 0;
}

static void decode_tvalues(const byte* buffer, struct TMtcPoint* point){

}

void decode_tmtc(const char* filename, struct TMtcObject* object) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL){
        perror("Failed to open file!");
        return;
    }

    byte* buffer = malloc(LOG_SIZE * CHUNK_SIZE);
    if(buffer == NULL){
        perror("Failed to allocate buffer!");
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_END);
#ifdef unix
    object->_file_length = ftell(fp);
#else
    object->_file_length = _ftelli64(fp);
#endif
    fseek(fp, 0, SEEK_SET);

    size_t bytesRead = 0;
    // Do macro multiplication happen at compile time?
    while((bytesRead = fread(buffer, 1, LOG_SIZE * CHUNK_SIZE, fp)) > 0){
        if (bytesRead != LOG_SIZE * CHUNK_SIZE){
            // we can maybe optimise this and not loose as much data, corruption maybe isn't even possible for a single write
            // however CHUNK_SIZE multiplication will lose data
            break;
        }

        uint8_t overshot_offset = decode_tchunk(buffer, object);
        // move fp back by ^ since we don't know size, gets weird with chunk sizing
        if(fseek(fp, -overshot_offset, SEEK_CUR) != 0){
            perror("Failed to shift overshot offset");
            goto cleanUpFunction;
        }
    }

    void* new_points_ptr = realloc(object->points, object->size * sizeof(struct TMtcPoint));
    if (new_points_ptr == NULL){
        perror("Failed to shrink point buffer!");
        goto cleanUpFunction;
    }
    object->points = new_points_ptr;

cleanUpFunction:
    free(buffer);
    fclose(fp);
}
