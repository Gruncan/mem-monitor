
#ifndef MEM_MONITOR_CONFIG_H
#define MEM_MONITOR_CONFIG_H
#include <stdint.h>

#define MASK_40 0xFFFFFFFFFF
#define MASK_32 0xFFFFFFFF
#define MASK_24 0xFFFFFF
#define MASK_16 0xFFFF
#define MASK_12 0x0FFF
#define MASK_8S 0x7F
#define MASK_8 0xFF
#define MASK_6 0x3F
#define MASK_5 0x1F
#define MASK_4 0x0F


#define MALLOC 0x0
#define CALLOC 0x1
#define REALLOC 0x2
#define REALLOC_ARRAY 0x3
#define FREE 0x4
#define NEW 0x5
#define NEW_NOTHROW 0x6
#define NEW_ARRAY 0x7
#define NEW_ARRAY_NOTHROW 0x8
#define DELETE 0x9
#define DELETE_SIZED 0xa
#define DELETE_NOTHROW 0xb
#define DELETE_ARRAY 0xc
#define DELETE_ARRAY_SIZED 0xd
#define DELETE_ARRAY_NOTHROW 0xe
#define NEW_ALIGN 0xf
#define NEW_ARRAY_ALIGN 0x10
#define DELETE_ALIGN 0x11
#define DELETE_ARRAY_ALIGN 0x12

#define MALLOC_FORMAT_STR "malloc(%zd) = %p\n"
#define CALLOC_FORMAT_STR "calloc(%zd, %zd) = %p\n"
#define REALLOC_FORMAT_STR "realloc(%p, %zd) = %p\n"
#define REALLOC_ARRAY_FORMAT_STR "reallocarray(%p, %zd, %zd) = %pan\n"
#define FREE_FORMAT_STR "free(%p)\n"
#define NEW_FORMAT_STR "new(%zd) = %p\n"
#define NEW_NOTHROW_FORMAT_STR "new_nothrow(%zd) = %p\n"
#define NEW_ARRAY_FORMAT_STR "new[](%zd) = %p\n"
#define NEW_ARRAY_NOTHROW_FORMAT_STR "new_nothrow[](%zd) = %p\n"
#define DELETE_FORMAT_STR "delete(%p)\n"
#define DELETE_SIZED_FORMAT_STR "delete_sized(%p, %lu)\n"
#define DELETE_NOTHROW_FORMAT_STR "delete_nothrow(%p)\n"
#define DELETE_ARRAY_FORMAT_STR "delete[](%p)\n"
#define DELETE_ARRAY_SIZED_FORMAT_STR "delete[](%p, %zd)\n"
#define DELETE_ARRAY_NOTHROW_FORMAT_STR "delete_nothrow[](%p)\n"
#define NEW_ALIGN_FORMAT_STR "new_align(%zd, %zd) = %p\n"
#define NEW_ARRAY_ALIGN_FORMAT_STR "new[]_align(%zd, %zd) = %p\n"
#define DELETE_ALIGN_FORMAT_STR "delete_align(%p, %zd)\n"
#define DELETE_ARRAY_ALIGN_FORMAT_STR "delete_align[](%p, %zd)\n"

#define MTC_V1_VALUE_WRITE_OFFSET 3
#define MTC_V3_VALUE_WRITE_OFFSET 4
#define MTC_V5_VALUE_WRITE_OFFSET 5

static uint8_t MTC_WRITE_OFFSET[] = {MTC_V1_VALUE_WRITE_OFFSET, MTC_V1_VALUE_WRITE_OFFSET,
                                    MTC_V3_VALUE_WRITE_OFFSET, MTC_V3_VALUE_WRITE_OFFSET,
                                    MTC_V5_VALUE_WRITE_OFFSET, MTC_V5_VALUE_WRITE_OFFSET,
};

#ifndef VERSION_3
typedef uint32_t mtc_point_size_t;

#define LOAD_MTC_VALUE_DATA(buffer, index)                                                                             \
  (((mtc_point_size_t) (buffer)[(index) + 1] << 24) | \
   ((mtc_point_size_t) (buffer)[(index) + 2] << 16) |              \
   ((mtc_point_size_t) (buffer)[(index) + 3] << 8)  | \
      ((buffer)[(index) + 4])) \

#define WRITE_MTC_VALUE_DATA(dest, value)   \
    (dest)[1] = (byte_t) ((value) >> 24) & MASK_8;   \
    (dest)[2] = (byte_t) ((value) >> 16) & MASK_8;   \
    (dest)[3] = (byte_t) ((value) >> 8) & MASK_8;   \
    (dest)[4] = (byte_t) ((value) & MASK_8);   \

#define MTC_VALUE_WRITE_OFFSET 5

#elifndef VERSION_1
typedef uint32_t mtc_point_size_t;

#define LOAD_MTC_VALUE_DATA(buffer, index)                                                                             \
  (((mtc_point_size_t) (buffer)[(index) + 1] << 16) | \
   ((mtc_point_size_t) (buffer)[(index) + 2] << 8) |              \
      ((buffer)[(index) + 3]))   \

#define WRITE_MTC_VALUE_DATA(dest, value)   \
    (dest)[1] = (byte_t) ((value) >> 16) & MASK_8;   \
    (dest)[2] = (byte_t) ((value) >> 8) & MASK_8;   \
    (dest)[3] = (byte_t) ((value) & MASK_8);   \

#define MTC_VALUE_WRITE_OFFSET 4

#else
#define LOAD_MTC_VALUE_DATA(buffer, index)                                                                                 \
    (((mtc_point_size_t) (buffer)[(index) + 1] << 8) | ((buffer)[(index) + 2])) \

#define WRITE_MTC_VALUE_DATA(dest, value)   \
    (dest)[1] = (byte_t) ((value) >> 8) & MASK_8; \
    (dest)[2] = (byte_t) ((value) & MASK_8); \

typedef uint16_t mtc_point_size_t;

#define MTC_VALUE_WRITE_OFFSET 3
#endif

// Memory key size
typedef uint8_t mk_size_t;

typedef unsigned char byte_t;

#endif // MEM_MONITOR_CONFIG_H
