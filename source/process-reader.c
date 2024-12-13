

#include "process-reader.h"

#include <mem-info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>


#define STATM_FIELDS 6



int check_process_exists(pid_t pid) {
    if (kill(pid, 0) == 0) return 1;

    if (errno == ESRCH) return 0;


    perror("kill");
    return -1;
}

int init_process_info(struct sMemProcessInfo* pi, pid_t pid) {
    if (check_process_exists(pid) == 0) {
        perror("Process does not exist");
        return -1;
    }

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

    return 0;
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

void reset_process_info(struct sMemProcessInfo* pi) {
    if (pi == NULL) return;

    pi->size = -1;
    pi->resident = -1;
    pi->shared = -1;
    pi->text = -1;
    pi->data = -1;
    pi->dirty = -1;
}
