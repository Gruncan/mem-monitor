
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem-info.h"


#define BUFFER_SIZE 16384 // 16Kb

struct sMemMapping {
    const char *key;
    unsigned long *field;
};



void mem_parse_line(char* line, const struct sMemMapping* mappings) {
    char key[256];
    unsigned long value;
    char unit[32];

    int items = sscanf(line, "%255[^:]: %lu %31s", key, &value, unit);
    if (items < 2) {
        items = sscanf(line, "%255[^:]: %lu", &value);
    }

    if (items < 2) {
        return;
    }

    char *newline = strchr(key, '\n');
    if (newline) *newline = '\0';

    char *paren = strchr(key, '(');
    if (paren) *paren = '\0';

    for (int i = 0; mappings[i].key != NULL; i++) {
        if (strcmp(key, mappings[i].key) == 0) {
            *mappings[i].field = value;
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


    const struct sMemMapping memMappings[] = {
        {"MemTotal", &mi->total},
        {"MemFree", &mi->free},
        {"MemAvailable", &mi->available},
        {"Buffers", &mi->buffers},
        {"Cached", &mi->cached},
        {"SwapCached", &mi->swapCache},
        {"Active", &mi->active},
        {"Inactive", &mi->inActive},
        {"Active(anon)", &mi->activeAnon},
        {"Inactive(anon)", &mi->inActiveAnon},
        {"Active(file)", &mi->activeFile},
        {"Inactive(file)", &mi->inActiveFile},
        {"Unevictable", &mi->unevictable},
        {"Mlocked", &mi->mLocked},
        {"SwapTotal", &mi->swapTotal},
        {"SwapFree", &mi->swapFree},
        {"Zswap", &mi->zswap},
        {"Zswapped", &mi->zswapped},
        {"Dirty", &mi->dirty},
        {"Writeback", &mi->writeback},
        {"AnonPages", &mi->pagesAnon},
        {"Mapped", &mi->pageMapped},
        {"Shmem", &mi->shmem},
        {"KReclaimable", &mi->kreClaimable},
        {"Slab", &mi->slab},
        {"SReclaimable", &mi->srClaimable},
        {"SUnreclaim", &mi->sunReclaim},
        {"KernelStack", &mi->kernelStack},
        {"PageTables", &mi->pageTables},
        {"SecPageTables", &mi->secPageTables},
        {"NFS_Unstable", &mi->nfsUnstable},
        {"Bounce", &mi->bounce},
        {"WritebackTmp", &mi->writebackTmp},
        {"CommitLimit", &mi->commitLimit},
        {"Committed_AS", &mi->committedAllocs},
        {"VmallocTotal", &mi->vmallocTotal},
        {"VmallocUsed", &mi->vmallocUsed},
        {"VmallocChunk", &mi->vmallocChunk},
        {"Percpu", &mi->perCPU},
        {"HardwareCorrupted", &mi->hardwareCorrupted},
        {"AnonHugePages", &mi->hugePagesAnon},
        {"ShmemHugePages", &mi->hugePagesShmem},
        {"ShmemPmdMapped", &mi->pmdMappedShmem},
        {"FileHugePages", &mi->hugePagesFile},
        {"FilePmdMapped", &mi->pmdMappedFile},
        {"Unaccepted", &mi->unaccepted},
        {"HugePages_Total", &mi->hugePagesTotal},
        {"HugePages_Free", &mi->hugePagesFree},
        {"HugePages_Rsvd", &mi->hugePagesRsvd},
        {"HugePages_Surp", &mi->hugePagesSurp},
        {"Hugepagesize", &mi->hugePageSize},
        {"Hugetlb", &mi->hugePageTLB},
        {"DirectMap4k", &mi->directMap4k},
        {"DirectMap2M", &mi->directMap2M},
        {"DirectMap1G", &mi->directMap1G},
        {NULL, NULL} // To mark the end
    };


    char *line = strtok(content, "\n");
    while (line != NULL) {
        mem_parse_line(line, memMappings);
        line = strtok(NULL, "\n");
    }

    free(content);

}
