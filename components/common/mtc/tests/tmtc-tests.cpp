


#include "mem-monitor-config.h"
#include "mtccdecoder.h"
#include <chrono>
#include <fstream>
#include <iostream>

#include <cinttypes>
#include <tmtcdecoder.h>




void printPointSize(const struct TMtcPoint* point) {
    switch (point->key) {
        case MALLOC:
        case NEW:
        case NEW_NOTHROW:
        case NEW_ARRAY:
        case NEW_ARRAY_NOTHROW:
        case NEW_ALIGN:
        case NEW_ARRAY_ALIGN:
            if (point->values[1] > 419430400) {
                printf("Size: %lu\n", point->values[1]);
            }
            // point->values[0];
            break;
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
        case CALLOC:
        default:
            break;
    }
}




void print_point(const struct TMtcPoint* point, uint64_t timeoffset) {
    printf("[%lu] ", timeoffset);
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
    object.is_collapsable = 1;

    decode_tmtc(argv[1], &object);
    printf("%lu\n", object.size);
    uint64_t timestamp = 0;
    for (uint64_t i = 0; i < object.size; i++) {
        timestamp += object.points[i].time_offset;
        print_point(&object.points[i], timestamp);
    }


    return 0;
}