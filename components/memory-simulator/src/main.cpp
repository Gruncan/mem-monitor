



#include "tmtcdecoder.h"
#include <chrono>

#include "stdlib.h"
#include <cstdio>
#include <cstring>
#include <map>
#include <mem-monitor-config.h>
#include <unistd.h>

#include <iostream>

#define DEBUG_PRINT_OFFSET 10




#define DEBUG_PRINT(index) \
    for(uint64_t j = (index) - DEBUG_PRINT_OFFSET; j < (index) + DEBUG_PRINT_OFFSET; j++){ \
        if (j == (index)) printf(">  "); \
        print_point(&tmtc_object.points[j]); \
    } \



#define DOUBLE_FREE_CHECK(ptr) \
    if (addressMapping.count(ptr) == 0) break; \
    //     // fprintf(stderr, "Double free caught internally, this should not happen, allocation log has failed at index %lu.", i); \
    //     // DEBUG_PRINT(i) \
    //     // _exit(-1); \
    //     break; \
    // }\

#define DOUBLE_ALLOCATION_CHECK(ptr) \
    if (addressMapping.count(ptr) == 1) break; \

#define CLEAN_ADDRESS_MAP(ptr) \
    if (addressMapping.count(ptr) == 1){ \
        addressMapping.erase(ptr);\
    }

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
static TMtcObject tmtc_object;

enum SimulationSpeed parseSpeed(const char* speedStr) {
    if (strcmp(speedStr, "nodelay") == 0)
        return NO_DELAY;
    else if (strcmp(speedStr, "real") == 0)
        return REAL;

    return NO_DELAY;
}

void print_point(const struct TMtcPoint* point) {
    switch (point->key) {
        case MALLOC:
            printf(MALLOC_FORMAT_STR, point->values[1], point->values[0]);
            break;
        case CALLOC:
            printf(CALLOC_FORMAT_STR, point->values[0], point->values[1], point->values[2]);
            break;
        case REALLOC:
            printf(REALLOC_FORMAT_STR, point->values[0], point->values[1], point->values[2]);
            break;
        case REALLOC_ARRAY:
            printf(REALLOC_ARRAY_FORMAT_STR, point->values[0], point->values[1], point->values[2]);
            break;
        case FREE:
            printf(FREE_FORMAT_STR, point->values[0]);
            break;
        case NEW:
            printf(NEW_FORMAT_STR, point->values[1], point->values[0]);
            break;
        case NEW_NOTHROW:
            printf(NEW_NOTHROW_FORMAT_STR, point->values[1], point->values[0]);
            break;
        case NEW_ARRAY:
            printf(NEW_ARRAY_FORMAT_STR, point->values[1], point->values[0]);
            break;
        case NEW_ARRAY_NOTHROW:
            printf(NEW_ARRAY_NOTHROW_FORMAT_STR, point->values[1], point->values[0]);
            break;
        case DELETE:
            printf(DELETE_FORMAT_STR, point->values[0]);
            break;
        case DELETE_SIZED:
            printf(DELETE_SIZED_FORMAT_STR, point->values[0], point->values[1]);
            break;
        case DELETE_NOTHROW:
            printf(DELETE_NOTHROW_FORMAT_STR, point->values[0]);
            break;
        case DELETE_ARRAY:
            printf(DELETE_ARRAY_FORMAT_STR, point->values[0]);
            break;
        case DELETE_ARRAY_SIZED:
            printf(DELETE_ARRAY_SIZED_FORMAT_STR, point->values[0], point->values[1]);
            break;
        case DELETE_ARRAY_NOTHROW:
            printf(DELETE_ARRAY_NOTHROW_FORMAT_STR, point->values[0]);
            break;
        case NEW_ALIGN:
            printf(NEW_ALIGN_FORMAT_STR, point->values[1], point->values[0], point->values[2]);
            break;
        case NEW_ARRAY_ALIGN:
            printf(NEW_ARRAY_ALIGN_FORMAT_STR, point->values[1], point->values[0]);
            break;
        case DELETE_ALIGN:
            printf(DELETE_ALIGN_FORMAT_STR, point->values[0], point->values[1]);
            break;
        case DELETE_ARRAY_ALIGN:
            printf(DELETE_ARRAY_ALIGN_FORMAT_STR, point->values[0], point->values[1]);
            break;
        default:
            break;
    }
}

void simulatePoint(struct TMtcPoint* point) {
    switch (point->key) {
        case MALLOC:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(malloc(point->values[1]));
            break;
        case NEW:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new(point->values[1]));
            break;
        case NEW_NOTHROW:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new (point->values[1], std::nothrow));
            break;
        case NEW_ARRAY:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new[](point->values[1]));
            break;
        case NEW_ARRAY_NOTHROW:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new[](point->values[1], std::nothrow));
            break;
        case NEW_ALIGN:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new(point->values[1], CAST_ALIGN(point->values[2])));
            break;
        case NEW_ARRAY_ALIGN:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
            addressMapping[point->values[0]] = CAST_FROM_PTR(operator new[](point->values[1], CAST_ALIGN(point->values[2])));
            break;
        case REALLOC:
            PTR_NULL_CHECK(point->values[0]);
            DOUBLE_ALLOCATION_CHECK(point->values[0])
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

    createTMtcObject(&tmtc_object);
    printf("Loading file: %s into memory...\n", filename);
    decode_tmtc(filename, &tmtc_object);
    printf("Successfully loaded file: %s\n", filename);

    printf("Starting simulation...\n");
    uint64_t interation_count = 0;
    while (true) {
        for (i = 0; i < tmtc_object.size; i++) {
            struct TMtcPoint point = tmtc_object.points[i];
            simulatePoint(&point);
            if (speed == REAL) {
                usleep(point.time_offset);
            }
        }
        if (interation_count % 10 == 0) {
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::cout << "Timestamp (" << interation_count << "): " << now_time << std::endl;
            printf("Address space analysis (iter: %lu):\n", interation_count);
            printf("Memory leaks: %lu\n", addressMapping.size());
        }
        interation_count++;
        addressMapping.clear();
    }

}