



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
    if (addressMapping.count(ptr) == 0){ \
        fprintf(stderr, "Double free caught internally, this should not happen, allocation log has failed."); \
        _exit(-1); \
    }\

#define CLEAN_ADDRESS_MAP(ptr) \
    if (addressMapping.count(ptr) == 0){ \
        addressMapping.erase(ptr);\
    }\

#define PTR_NULL_CHECK(ptr) \
    if ((ptr) == 0) break;

#define CAST_TO_PTR(integer) (reinterpret_cast<void*>(integer))
#define CAST_FROM_PTR(ptr) (reinterpret_cast<uintptr_t>(ptr))
#define CAST_ALIGN(align) static_cast<std::align_val_t>(align)

enum SimulationSpeed {
    NO_DELAY,
    REAL,
};


static std::map<uintptr_t, uintptr_t> addressMapping = {};
static uint64_t i;

enum SimulationSpeed parseSpeed(const char* speedStr) {
    if (strcmp(speedStr, "nodelay") == 0)
        return NO_DELAY;
    else if (strcmp(speedStr, "real") == 0)
        return REAL;

    return NO_DELAY;
}

void simulatePoint(struct TMtcPoint* point) {
    switch (point->key) {
        case MALLOC:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(malloc(point->values[1]));
            break;
        case NEW:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new(point->values[1]));
            break;
        case NEW_NOTHROW:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new (point->values[1], std::nothrow));
            break;
        case NEW_ARRAY:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new[](point->values[1]));
            break;
        case NEW_ARRAY_NOTHROW:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new[](point->values[1], std::nothrow));
            break;
        case NEW_ALIGN:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new(point->values[1], CAST_ALIGN(point->values[2])));
            break;
        case NEW_ARRAY_ALIGN:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new[](point->values[1], CAST_ALIGN(point->values[2])));
            break;
        case REALLOC:
            PTR_NULL_CHECK(point->values[0]);
            addressMapping[point->values[2]] = CAST_FROM_PTR(realloc(CAST_TO_PTR(addressMapping[point->values[0]]), point->values[1]));
            break;
        case REALLOC_ARRAY:
            break; // TODO fix this requires fixing memory-tracker ;( is it even used?
        case FREE:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            free(CAST_TO_PTR(addressMapping[point->values[0]]));
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]));
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_SIZED:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]), point->values[1]);
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_NOTHROW:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]), std::nothrow);
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_ARRAY:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]));
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_ARRAY_SIZED:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]), point->values[1]);
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_ARRAY_NOTHROW:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]), std::nothrow);
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_ALIGN:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete(CAST_TO_PTR(addressMapping[point->values[0]]), CAST_ALIGN(point->values[1]));
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case DELETE_ARRAY_ALIGN:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_FREE_CHECK(point->values[0]);
            operator delete [](CAST_TO_PTR(addressMapping[point->values[0]]), CAST_ALIGN(point->values[1]));
            CLEAN_ADDRESS_MAP(point->values[0])
            break;
        case CALLOC:
            addressMapping[point->values[2]] = CAST_FROM_PTR(calloc(point->values[0], point->values[1]));
            break;
        default:
            fprintf(stderr, "Unknown allocation key: %d at log index %lu\n", point->key, i);
            break;
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./simulator <filename>.tmtc <nodelay|real>\n");
        return -1;
    }

    char* filename = argv[1];
    const char* speedStr = argv[2];
    const enum SimulationSpeed speed = parseSpeed(speedStr);

    TMtcObject object;
    createTMtcObject(&object);
    printf("Loading file: %s into memory...\n", filename);
    decode_tmtc(filename, &object);
    printf("Successfully loaded file: %s\n", filename);

    printf("Starting simulation...\n");

    for (i = 0; i < object.size; i++) {
        struct TMtcPoint point = object.points[i];
        simulatePoint(&point);
        if (speed == REAL) {
            usleep(point.time_offset);
        }
    }

    printf("\nSimulation complete.\n");
    printf("Address space analysis:\n");
    printf("Memory leaks: %lu\n", addressMapping.size());

    for (auto & it : addressMapping) {
        printf("\t- Sim Address: %p, Logged address: %p\n", CAST_TO_PTR(it.second), CAST_TO_PTR(it.first));
    }

    return 0;
}