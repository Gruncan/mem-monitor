
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem-info.h"


#define BUFFER_SIZE 16384 // 16Kb

struct sMemMapping {
    const char *key;
    unsigned long *field;
};


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


void mem_parse_line(char* line, struct sMemInfo* mi) {
    char key[256];
    unsigned long value;
    char unit[32];

    int items = sscanf(line, "%255[^:]: %lu %31s", key, &value, unit);
    if (items < 2) {
        items = sscanf(line, "%255[^:]: %lu", &value);
    }

    if (items < 2) {
        // We should always have a unit?
        return;
    }

    char *newline = strchr(key, '\n');
    if (newline) *newline = '\0';

    char *paren = strchr(key, '(');
    if (paren) *paren = '\0';

    const size_t structLength = sizeof(struct sMemInfo);
    const size_t parLength = sizeof(unsigned long);

    for (int i = 0; i < structLength / parLength ; i++) {
        if(strcmp(key, memMapping[i]) == 0) {
            const size_t valueOffset = i * parLength;
            if (valueOffset >= structLength) {
                perror("Failed to write to struct!");
                return;
            }
            // This looks disgusting.. I thought I understood C but pointer arithmetic needs casting to 1 byte char?
            *(unsigned long *)((char *)mi + valueOffset) = value;

            break;
        }
    }
}


void read_mem_info(struct sMemInfo* mi) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("Error opening /proc/meminfo");
    }


    size_t content_size = 0;
    char *content = malloc(BUFFER_SIZE);
    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return;
    }

    size_t bytes_read;
    while ((bytes_read = fread(content + content_size, 1, BUFFER_SIZE - content_size - 1, fp)) > 0) {
        content_size += bytes_read;
    }
    content[content_size] = '\0';

    fclose(fp);



    char *line = strtok(content, "\n");
    while (line != NULL) {
        mem_parse_line(line, mi);
        line = strtok(NULL, "\n");
    }

    free(content);

}
