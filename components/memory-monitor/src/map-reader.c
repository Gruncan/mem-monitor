

#include "map-reader.h"

#include <mem-info.h>


void init_mem_page_info(struct mem_page_info_s* mem_page_info) {
    mem_page_info->start_address = 0;
    mem_page_info->end_address = 0;
    mem_page_info->permissions = malloc(4);
    memset(mem_page_info->permissions, NO_PERMISSION, 4);
    mem_page_info->mapping_offset = 0;
    mem_page_info->device_major = 0;
    mem_page_info->device_minor = 0;
    mem_page_info->inode = 0;
    mem_page_info->pathname = NULL;
}

void init_mem_map_info(MemMapInfo* mem_map_info) {
    mem_map_info->_page_info_size = 8;
    mem_map_info->mem_page_info = malloc(sizeof(struct mem_page_info_s*) * mem_map_info->_page_info_size);
    mem_map_info->page_count = 0;
}

inline void write_mem_page_info(MemMapInfo* mem_map_info, struct mem_page_info_s* mem_page_info) {
    if (mem_map_info->page_count - 1 == mem_map_info->_page_info_size) {
        mem_map_info->_page_info_size *= 2;
        struct mem_page_info_s** new_mem_page_info = realloc(mem_map_info->mem_page_info, sizeof(struct mem_page_info_s*) * mem_map_info->_page_info_size);
        if (new_mem_page_info == NULL) {
            perror("Failed to reallocate memory page info");
        }
        mem_map_info->mem_page_info = new_mem_page_info;

    }
    mem_map_info->mem_page_info[mem_map_info->page_count++] = mem_page_info;
}


void map_parse_line(const char* line, MemMapInfo* mem_map_info) {
    char start_address[17];
    char end_address[17];

    char* pathname = malloc(256);
    char permissions[5];

    struct mem_page_info_s* mem_page_info = malloc(sizeof(struct mem_page_info_s));
    init_mem_page_info(mem_page_info);


    int items = sscanf(line, "%16[^-]-%16s %4s %lx %u:%u %lu %255[^\n]",
                                        start_address, end_address, permissions, &mem_page_info->mapping_offset,
                                        &mem_page_info->device_major, &mem_page_info->device_minor, &mem_page_info->inode,
                                        pathname);
    if (items != 8 && items != 7) {
        perror("Failed to parse line");
        free(pathname);
        return;
    }
    mem_page_info->start_address = strtoul(start_address, NULL, 16);
    mem_page_info->end_address = strtoul(end_address, NULL, 16);
    mem_page_info->pathname = pathname;

    for (int i=0; i < 4; i++) {
        switch (permissions[i]) {
            case 'r':
                mem_page_info->permissions[i] = READ;
                break;
            case 'w':
                mem_page_info->permissions[i] = WRITE;
                break;
            case 'x':
                mem_page_info->permissions[i] = EXEC;
                break;
            case 's':
                mem_page_info->permissions[i] = SHARED;
                break;
            case 'p':
                mem_page_info->permissions[i] = PRIVATE;
                break;
            default:
                break;
        }
    }
    write_mem_page_info(mem_map_info, mem_page_info);
}

void read_map_info(MemMapInfo* mem_map_info, const pid_t pid) {
    char file[128];
    sprintf(file, "/proc/%d/maps", pid);
    char* content = mem_parse_file(file, 1024, READ_RAW);

    if (content == NULL) {
        perror("Failed maps read\n");
        return;
    }

    if (strlen(content) == 0) {
        // Set to default here.
        goto cleanup;
    }

    char* line = strtok(content, "\n");
    while (line != NULL) {
        map_parse_line(line, mem_map_info);
        line = strtok(NULL, "\n");
    }

cleanup:
    free(content);
}



#ifdef PROCMAP_QUERY


#endif
