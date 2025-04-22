


#include "mem-monitor-config.h"
#include "mtccdecoder.h"
#include <chrono>
#include <fstream>
#include <iostream>

#include <cinttypes>
#include <map>
#include <tmtcdecoder.h>




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




void print_point(const struct TMtcPoint* point, uint64_t timeoffset) {
#ifndef MEM_TEST
    printf("[%lu] ", timeoffset);
#endif
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
            printf(NEW_ALIGN_FORMAT_STR, point->values[1], point->values[2], point->values[0]);
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return -1;
    }


    TMtcObject object;

    createTMtcObject(&object);
#ifdef MEM_TEST
    object.is_collapsable = 0;
#endif

    decode_tmtc(argv[1], &object);

#ifndef MEM_TEST
    printf("%lu\n", object.size);
    uint64_t timestamp = 0;
    std::map<uint64_t, struct TMtcPoint*> address_map;
    std::map<uint64_t, uint64_t> address_timestamp;
    for (uint64_t i = 0; i < object.size; i++) {
        timestamp += object.points[i].time_offset;
        uint64_t address = getTMtcPointAddress(&object.points[i]);
        switch (object.points[i].key) {
            case MALLOC:
            case NEW:
            case NEW_NOTHROW:
            case NEW_ARRAY:
            case NEW_ARRAY_NOTHROW:
            case NEW_ALIGN:
            case NEW_ARRAY_ALIGN:
            case REALLOC:
            case REALLOC_ARRAY:
            case CALLOC:
                if (address_map.count(address) != 0) {
                    break;
                }
                address_map[address] = &object.points[i];
                address_timestamp[address] = timestamp;
                break;
            case FREE:
            case DELETE:
            case DELETE_SIZED:
            case DELETE_NOTHROW:
            case DELETE_ARRAY:
            case DELETE_ARRAY_SIZED:
            case DELETE_ARRAY_NOTHROW:
            case DELETE_ALIGN:
            case DELETE_ARRAY_ALIGN:
                if (address_map.count(address) == 0) {
                    break;
                }
                address_map.erase(address);
                address_timestamp.erase(address);
                //address_timestamp[address] = timestamp - address_timestamp[address];
                break;
        }
    }

    for (const auto& pair : address_map) {
        print_point(pair.second, address_timestamp[pair.first]);
    }

#else
    for (uint64_t i = 0; i < object.size; i++) {
        print_point(&object.points[i], 0);
    }
#endif

    return 0;
}