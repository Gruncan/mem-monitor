
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem-info.h"


#define BUFFER_SIZE 16384 // 16Kb. This should be more than enough for big systems,
                          //        might need to tweek for lower systems..

static const size_t sizeUL = sizeof(unsigned long);

static const char* memMapping[] = {
    "MemTotal", "MemFree", "MemAvailable", "Buffers", "Cached", "SwapCached",
    "Active"," Inactive", "Active(anon)", "Inactive(anon)","Active(file)",
    "Inactive(file)", "Unevictable", "Mlocked", "SwapTotal", "SwapFree", "Zswap",
    "Zswapped", "Dirty", "Writeback", "AnonPages", "Mapped", "Shmem", "KReclaimable",
    "Slab", "SReclaimable", "SUnreclaim", "KernelStack", "PageTables", "SecPageTables",
    "NFS_Unstable", "Bounce", "WritebackTmp", "CommitLimit", "Committed_AS", "VmallocTotal",
    "VmallocUsed", "VmallocChunk", "Percpu", "HardwareCorrupt", "AnonHugePages",
    "ShmemHugePages", "ShmemPmdMapped", "FileHugePages", "FilePmdMapped", "Unaccepted",
    "HugePages_Total", "HugePages_Free", "HugePages_Rsvd", "HugePages_Surp", "Hugepagesize",
    "Hugetlb", "DirectMap4k", "DirectMap2M", "DirectMap1G",
};

static const char* memPageMapping[] = {
    "pgfault", "pgmajfault", "pgpgin", "pgpgout"
};


static const char* memInfoNames[] = {
    "total", "free", "available", "buffers", "cached", "swapCache", "active", "inActive", "activeAnon", "inActiveAnon",
    "activeFile", "inActiveFile", "unevictable", "mLocked", "swapTotal", "swapFree",
    "zswap", "zswapped", "dirty", "writeback", "pagesAnon", "pageMapped", "shmem", "kreClaimable", "slab",
    "srClaimable",
    "sunReclaim", "kernelStack", "pageTables", "secPageTables", "nfsUnstable", "bounce", "writebackTmp", "commitLimit",
    "committedAllocs", "vmallocTotal", "vmallocUsed", "vmallocChunk", "perCPU", "hardwareCorrupted", "hugePagesAnon",
    "hugePagesShmem", "pmdMappedShmem", "hugePagesFile", "pmdMappedFile", "unaccepted", "hugePagesTotal",
    "hugePagesFree",
    "hugePagesRsvd", "hugePagesSurp", "hugePageSize", "hugePageTLB", "directMap4k", "directMap2M", "directMap1G"
};



char* get_ulong_str(const unsigned long ulong_value) {
    const int length = snprintf(NULL, 0, "%lu", ulong_value);

    char *buf = malloc(length + 1);
    if (buf == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    snprintf(buf, length + 1, "%lu", ulong_value);

    return buf;
}


void set_mem_struct_value(void* sStruct, const size_t structLength,
                          const char* map[], const char* key, const unsigned long value) {
    for (int i = 0; i < structLength / sizeUL; i++) {
        if(strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * sizeUL;
            if (valueOffset >= structLength) {
                perror("Failed to write to struct!");
                return;
            }
            // This looks disgusting.. I thought I understood C but pointer arithmetic needs casting to 1 byte char?
            *(unsigned long*)((char*)sStruct + valueOffset) = value;

            break;
        }
    }
}

unsigned long get_mem_struct_value(void* sStruct, const size_t structLength, const char* map[], const char* key) {
    for (int i = 0; i < structLength / sizeUL; i++) {
        if (strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * sizeUL;
            if (valueOffset >= structLength) {
                perror("Failed to read from struct!");
            }
            return *(unsigned long*) ((char*) sStruct + valueOffset);
        }
    }

    return -1; // Error memory stat should not be possible for negative?
}

struct memInfoStrings* get_all_mem_struct_values(const unsigned long* values, const size_t valuesLength) {
    char** buffer = malloc(valuesLength * sizeof(char*));
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    for (int i = 0; i < valuesLength / sizeUL; i++) {
        unsigned long v = *(unsigned long*) ((char*) values + (i * sizeUL));
        buffer[i] = get_ulong_str(v);
    }

    struct memInfoStrings* ms = malloc(sizeof(struct memInfoStrings));

    ms->mem_strings = buffer;
    ms->mem_strings_count = valuesLength / sizeUL;

    return ms;
}



void mem_parse_page_line(const char* line, struct sMemPageInfo* mp) {
    char key[256];
    unsigned long value;
    int items = sscanf(line, "%255s %lu", key, &value);
    if (items != 2) {
        // Something has gone wrong..
        return;
    }

    char* newline = strchr(key, '\n');
    if (newline) *newline = '\0';

    const size_t structLength = sizeof(struct sMemPageInfo);

    set_mem_struct_value(mp, structLength, memPageMapping, key, value);

}

void mem_parse_line(const char* line, struct sMemInfo* mi) {
    char key[256];
    unsigned long value;
    char unit[32];

    int items = sscanf(line, "%255[^:]: %lu %31s", key, &value, unit);
    if (items < 2) {
        items = sscanf(line, "%255[^:]: %lu", key, &value);
    }

    if (items < 2) {
        // We should always have a unit?
        return;
    }

    char* newline = strchr(key, '\n');
    if (newline) *newline = '\0';

    char* paren = strchr(key, '(');
    if (paren) *paren = '\0';

    const size_t structLength = sizeof(struct sMemInfo);

    set_mem_struct_value(mi, structLength, memMapping, key, value);

}


char* mem_parse_file(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }


    size_t content_size = 0;
    char* content = malloc(BUFFER_SIZE);
    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    size_t bytes_read;
    while ((bytes_read = fread(content + content_size, 1, BUFFER_SIZE - content_size - 1, fp)) > 0) {
        content_size += bytes_read;
    }
    content[content_size] = '\0';

    fclose(fp);

    return content;
}


void read_mem_info(struct sMemInfo* mi) {
    char* content = mem_parse_file("/proc/meminfo");
    if (content == NULL) {
        perror("Failed to access /proc/meminfo");
        return;
    }

    char* line = strtok(content, "\n");
    while (line != NULL) {
        mem_parse_line(line, mi);
        line = strtok(NULL, "\n");
    }

    free(content);

}

void read_mem_pages(struct sMemPageInfo* mp) {
    char* content = mem_parse_file("/proc/vmstat");
    if (content == NULL) {
        perror("Failed to access /proc/vmstat");
        return;
    }

    char *line = strtok(content, "\n");
    while (line != NULL) {
        mem_parse_page_line(line, mp);
        line = strtok(NULL, "\n");
    }

    free(content);
}


char* get_mem_page_data(struct sMemPageInfo* mp, const char *name) {
    const unsigned long value = get_mem_struct_value(mp, sizeof(struct sMemPageInfo), memPageMapping, name);
    if (value < 0) {
        return NULL;
    }

    return get_ulong_str(value);
}

struct memInfoStrings* get_all_mem_page_data(struct sMemPageInfo* mp) {
    return get_all_mem_struct_values((unsigned long*) mp, sizeof(struct sMemPageInfo));
}

char* get_mem_info_data(struct sMemInfo* mi, const char* name) {
    const unsigned long value = get_mem_struct_value(mi, sizeof(struct sMemInfo), memInfoNames, name);
    if (value < 0) {
        return NULL;
    }

    return get_ulong_str(value);
}

struct memInfoStrings* get_all_mem_info_data(struct sMemInfo* mi) {
    return get_all_mem_struct_values((unsigned long*) mi, sizeof(struct sMemInfo));
}

void destroy_all_mem_data(struct memInfoStrings* ms) {
    for (int i=0; i < ms->mem_strings_count; i++) {
        free(ms->mem_strings[i]);
    }
    free(ms->mem_strings);
    free(ms);

}

char** get_mem_info_names() {
    return memInfoNames;
}

char** get_mem_page_names() {
    return memPageMapping;
}
