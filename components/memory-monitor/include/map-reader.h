
#ifndef MAP_READER_H
#define MAP_READER_H

#include "mem-writer.h"

enum page_permission_e {
    READ,
    WRITE,
    EXEC,
    SHARED,
    PRIVATE,
    NO_PERMISSION
};

enum page_area_e {
    HEAP,
    STACK,
    VDSO,
    PRIVATE_ANON,
    SHARED_ANON,
    ANON,
    NO_AREA
};

struct mem_page_info_s {
    ulong start_address;
    ulong end_address;
    enum page_permission_e* permissions;
    ulong mapping_offset;
    uint device_major;
    uint device_minor;
    ulong inode;
    char* pathname;

};

typedef struct mem_map_info_s {
    struct mem_page_info_s** mem_page_info;
    uint page_count;
    uint _page_info_size;
} MemMapInfo;

void init_mem_page_info(struct mem_page_info_s* mem_page_info);

void init_mem_map_info(MemMapInfo* mem_page_info);

void map_parse_line(const char* line, MemMapInfo* mem_map_info);

void read_map_info(MemMapInfo* mem_map_info, pid_t pid);

void write_mem_page_info(MemMapInfo* mem_map_info, struct mem_page_info_s* mem_page_info);

#endif //MAP_READER_H
