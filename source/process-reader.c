

#include "process-reader.h"

#include <mem-info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define STATM_FIELDS 6


void init_process_info(struct sMemProcessInfo* pi) {
    pi->oomAdj = -1;
    pi->oomScore = -1;
    pi->oomScoreAdj = -1;
    pi->data = 0;
    pi->size = 0;
    pi->resident = 0;
    pi->shared = 0;
    pi->text = 0;
    pi->data = 0;
    pi->dirty = 0;
}



void read_process_mem_info(struct sMemProcessInfo* pi, const pid_t pid) {
    char file[256];
    snprintf(file, sizeof(file), "/proc/%d/statm", pid);

    char* content = mem_parse_file(file, 64, READ_BINARY);
    if (content == NULL) {
        printf("mem_parse_file failed\n");
        return;
    }

    if (strlen(content) == 0) {
        pi->size = 0;
        pi->resident = 0;
        pi->shared = 0;
        pi->text = 0;
        pi->data = 0;
        pi->dirty = 0;
    }else {
        if(sscanf(content, "%lu %lu %lu %lu %lu %lu",
               &pi->size, &pi->resident, &pi->shared, &pi->text, &pi->data, &pi->dirty) != STATM_FIELDS) {
            perror("Failed to parse /proc/pid/statm");
            free(content);
            return;
        }

        const long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;

        pi->size *= page_size_kb;
        pi->resident *= page_size_kb;
        pi->shared *= page_size_kb;
        pi->text *= page_size_kb;
        pi->data *= page_size_kb;
        pi->dirty *= page_size_kb;
    }

    free(content);
}

void read_process_info(struct sMemProcessInfo* pi, const pid_t pid){
    const size_t length = 3;
    char* files[] = {"/proc/%d/oom_adj", "/proc/%d/oom_score", "/proc/%d/oom_score_adj"};

    char filenames[length][20];

    for (int i = 0; i < length; i++) {
        sprintf(filenames[i], files[i], pid);
        char* content = mem_parse_file(filenames[i], 8, READ_RAW);
        const int value = atoi(content);
        switch (i) {
            case 0:
                pi->oomAdj = value;
            break;
            case 1:
                pi->oomScore = value;
            break;
            case 2:
                pi->oomScoreAdj = value;
            break;
            default:
                break;
        }
        free(content);
    }

    read_process_mem_info(pi, pid);
}

void reset_mem_info(struct sMemProcessInfo* info) {
    if (info == NULL) return;

    info->size = -1;
    info->resident = -1;
    info->shared = -1;
    info->text = -1;
    info->data = -1;
    info->dirty = -1;
}


void reset_process_info(struct sMemProcessInfo* info) {
    if (info == NULL) return;

    info->oomAdj = -1;
    info->oomScore = -1;
    info->oomScoreAdj = -1;

    reset_mem_info(info);
}
