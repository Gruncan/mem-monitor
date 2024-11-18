

#include "process-reader.h"

#include <mem-info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define STATM_FIELDS 6
#define STAT_FIELDS 3

static const char* processMemInfoNames[] = {
    "p_size", "p_resident", "p_shared", "p_text", "p_data", "p_dirty"
};


static const char* processMemInfoStatNames[] = {
    "p_vsize", "p_major_faults", "p_minor_faults"
};


void init_process_info(struct sProcessInfo* pi, pid_t pid) {
    pi->pid = pid;
    pi->name = NULL;
    pi->oomAdj = -1;
    pi->oomScore = -1;
    pi->oomScoreAdj = -1;
    pi->memInfo = malloc(sizeof(struct sProcessInfo));
    pi->memStat = malloc(sizeof(struct sProcessMemStat));
}



void read_process_mem_info(const pid_t pid, struct sProcessMem* pm, struct sProcessMemStat* pms) {
    char file[256];
    snprintf(file, sizeof(file), "/proc/%d/statm", pid);

    char* content = mem_parse_file(file, 64, READ_BINARY);
    if (content == NULL) {
        printf("mem_parse_file failed\n");
        return;
    }

    if (strlen(content) == 0) {
        pm->size = 0;
        pm->resident = 0;
        pm->shared = 0;
        pm->text = 0;
        pm->data = 0;
        pm->dirty = 0;
    }else {
        if(sscanf(content, "%lu %lu %lu %lu %lu %lu",
               &pm->size, &pm->resident, &pm->shared, &pm->text, &pm->data, &pm->dirty) != STATM_FIELDS) {
            perror("Failed to parse /proc/pid/statm");
            free(content);
            return;
        }

        const long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;

        pm->size *= page_size_kb;
        pm->resident *= page_size_kb;
        pm->shared *= page_size_kb;
        pm->text *= page_size_kb;
        pm->data *= page_size_kb;
        pm->dirty *= page_size_kb;
    }
    free(content);

    snprintf(file, sizeof(file), "/proc/%d/stat", pid);
    content = mem_parse_file(file, 512, READ_BINARY);
    if (content == NULL) {
        printf("mem_parse_file failed\n");
    }

    if (strlen(content) == 0) {
        pms->vsize = 0;
        pms->major_faults = 0;
        pms->minor_faults = 0;
    }else {
        if (sscanf(content, "%*d %*s %*c %*d %*d %*d %*d %*d %*d %ld %ld %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %ld %ld",
               &pms->vsize, &pms->major_faults, & pms->minor_faults) != 3) {
            perror("Error parsing stat data");
        }
    }
    free(content);
}

void read_process_info(struct sProcessInfo* pi){
    const size_t length = 3;
    char* files[] = {"/proc/%d/oom_adj", "/proc/%d/oom_score", "/proc/%d/oom_score_adj"};

    char filenames[length][20];

    for (int i = 0; i < length; i++) {
        sprintf(filenames[i], files[i], pi->pid);
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

    read_process_mem_info(pi->pid, pi->memInfo, pi->memStat);
}

void reset_mem_info(struct sProcessMem* info) {
    if (info == NULL) return;

    info->size = -1;
    info->resident = -1;
    info->shared = -1;
    info->text = -1;
    info->data = -1;
    info->dirty = -1;
}

void reset_mem_stat(struct sProcessMemStat* info) {
    if (info == NULL) return;

    info->vsize = -1;
    info->major_faults = -1;
    info->minor_faults = -1;
}


void reset_process_info(struct sProcessInfo* info) {
    if (info == NULL) return;

    info->oomAdj = -1;
    info->oomScore = -1;
    info->oomScoreAdj = -1;
    info->pid = -1;

    reset_mem_info(info->memInfo);
}


struct memInfoStrings* get_process_mem_info_names(struct sProcessMem* pm) {
    return get_all_mem_struct_values((unsigned long*) pm, sizeof(struct sProcessMem));
}

struct memInfoStrings* get_process_mem_stat_info_names(struct sProcessMemStat* pms) {
    return get_all_mem_struct_values((unsigned long*) pms, sizeof(struct sProcessMemStat));
}

const char** get_process_mem_names() {
    return processMemInfoNames;
}

const char** get_process_mem_stat_names() {
    return processMemInfoStatNames;
}
