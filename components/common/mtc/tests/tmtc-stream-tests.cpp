


#include <cstdio>
#include <mem-monitor-config.h>
#include <tmtcdecoder.h>

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


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    TMtcStream stream;
    createTMtcStream(&stream);
    stream_decode_tmtc(argv[1], &stream, 0);

    struct TMtcObject* object;
    do {
        object = stream_tmtc_next(&stream);
        if (object == NULL) {
            continue;
        }

        for (int j = 0; j < object->size; j++) {
            print_point(&object->points[j]);
        }
    } while (object != NULL);

    return 0;
}