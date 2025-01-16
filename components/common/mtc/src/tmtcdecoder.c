
#include <stdio.h>
#include <malloc.h>
#include "tmtcdecoder.h"
#include "mtc-config.h"


#define LOG_SIZE 13
#define CHUNK_SIZE 1

#define MAX_LOG_VARS 3

typedef unsigned char byte;


#define ARRAY_COMBINE4(array, index) ((array)[index] << 24) | ((array)[index + 1] << 16) | ((array)[index + 2] << 8) | ((array)[index + 3])


inline void initaliseTMtcObject(struct TMtcObject* object) {

}

inline uint8_t queryTDecodeProgress(struct TMtcObject* object) {
    return 0;
}

static uint8_t decode_tchunk(const byte* buffer, struct TMtcObject* object){
    uint64_t seconds = ARRAY_COMBINE4(buffer, 0);
    uint64_t micro_seconds = ARRAY_COMBINE4(buffer, 4);
    object->points[object->size].key = buffer[8];
    uint8_t length = buffer[9];
    object->points[object->size].length = length;
    object->points[object->size].values = calloc(0, length);
    uint64_t* values = object->points[object->size].values;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 8; ++j) {
            values[i] |= buffer[(i * 8) + 10 + j] << (8 * (7 - j));
        }
    }

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
#ifdef UNIX
    object->_file_length = ftell(fp)
#else
//    object->_file_length = _ftelli64(fp);
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

    }

    free(buffer);
}
