



#include "tmtcdecoder.h"
#include <argp.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <mem-monitor-config.h>
#include <ostream>
#include <unistd.h>


#define DOUBLE_FREE_CHECK(ptr) \
    if (addressMapping.contains(ptr)){ \
        fprintf(stderr, "Double free caught internally, this should not happen, allocation log has failed."); \
        _exit(-1); \
    }\

#define CAST_TO_PTR(integer) (reinterpret_cast<void*>(integer))
#define CAST_FROM_PTR(ptr) (reinterpret_cast<uintptr_t>(ptr))
#define CAST_ALIGN(align) static_cast<std::align_val_t>(align)

enum SimulationSpeed {
    NO_DELAY,
    REAL,
};


std::map<uintptr_t, uintptr_t> addressMapping = {};


enum SimulationSpeed parseSpeed(const char* speedStr) {
    if (strcmp(speedStr, "nodelay") == 0)
        return NO_DELAY;
    else if (strcmp(speedStr, "real") == 0)
        return REAL;
}

void simulatePoint(struct TMtcPoint* point) {
    switch (point->key) {
        case MALLOC:
            addressMapping[point->values[1]] = CAST_FROM_PTR(malloc(point->values[0]));
            break;
        case NEW:
            addressMapping[point->values[1]] = CAST_FROM_PTR(operator new(point->values[0]));
            break;
        case NEW_NOTHROW:
            addressMapping[point->values[1]] = CAST_FROM_PTR(operator new (point->values[0], std::nothrow));
            break;
        case NEW_ARRAY:
            addressMapping[point->values[1]] = CAST_FROM_PTR(operator new[](point->values[0]));
            break;
        case NEW_ARRAY_NOTHROW:
            addressMapping[point->values[1]] = CAST_FROM_PTR(operator new[](point->values[0], std::nothrow));
            break;
        case NEW_ALIGN:
            addressMapping[point->values[1]] = CAST_FROM_PTR(operator new(point->values[0], CAST_ALIGN(point->values[2])));
            break;
        case NEW_ARRAY_ALIGN:
            addressMapping[point->values[1]] = CAST_FROM_PTR(operator new[](point->values[0], CAST_ALIGN(point->values[2])));
            break;
        case REALLOC:
            addressMapping[point->values[2]] = CAST_FROM_PTR(realloc(CAST_TO_PTR(addressMapping[point->values[0]]), point->values[1]));
            break;
        case REALLOC_ARRAY:
            break; // TODO fix this requires fixing memory-tracker ;( is it even used?
        case FREE:
            DOUBLE_FREE_CHECK(point->values[0]);
            free(CAST_TO_PTR(addressMapping[point->values[0]]));
            break;
        case DELETE:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]));
            break;
        case DELETE_SIZED:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]), point->values[1]);
            break;
        case DELETE_NOTHROW:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]), std::nothrow);
            break;
        case DELETE_ARRAY:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]));
            break;
        case DELETE_ARRAY_SIZED:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]), point->values[1]);
            break;
        case DELETE_ARRAY_NOTHROW:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]), std::nothrow);
            break;
        case DELETE_ALIGN:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]), CAST_ALIGN(point->values[1]));
            break;
        case DELETE_ARRAY_ALIGN:
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]), CAST_ALIGN(point->values[1]));
            break;
        case CALLOC:
            addressMapping[point->values[2]] = CAST_FROM_PTR(calloc(point->values[0], point->values[1]));
            break;
        default:
            return;
    }
}


int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage: ./simulator <filename>.tmtc <nodelay|real>\n");
    }

    char* filename = argv[1];
    char* speedStr = argv[2];
    enum SimulationSpeed speed = parseSpeed(speedStr);

    TMtcObject object;
    createTMtcObject(&object);
    printf("Loading file: %s into memory...\n", filename);
    decode_tmtc(filename, &object);
    printf("Successfully loaded file: %s\n", filename);

    printf("Starting simulation...\n");
    for (uint64_t i = 0; i < object.size; i++) {
        struct TMtcPoint point = object.points[i];
        simulatePoint(&point);
        if (speed == REAL) {
            usleep(point.time_offset * 1000);
        }
    }

    printf("\nSimulation complete.\n");

    return 0;
}