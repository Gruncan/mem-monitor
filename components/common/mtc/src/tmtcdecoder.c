
#include "tmtcdecoder.h"

typedef unsigned char byte;

inline void initaliseTMtcObject(struct TMtcObject* object) {

}

inline uint8_t queryTDecodeProgress(struct TMtcObject* object) {
    return 0;
}

static void decode_tchunk(const byte* buffer, struct TMtcObject* object){

}

static void decode_tvalues(const byte* buffer, struct TMtcPoint* point){

}

void decode_tmtc(const char* filepath, struct TMtcObject* object) {

}
