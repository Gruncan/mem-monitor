

#include "process-reader.h"

#include <mem-info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



void init_process_info(struct sProcessInfo* pi, pid_t pid) {
    pi->pid = pid;
    pi->name = NULL;
    pi->oomAdj = -1;
    pi->oomScore = -1;
    pi->oomScoreAdj = -1;
    pi->memInfo = malloc(sizeof(struct sProcessInfo));
}


void parse_oom_data(struct sProcessInfo* pi) {
    const size_t length = 3;
    char* files[] = {"/proc/%d/oom_adj", "/proc/%d/oom_score", "/proc/%d/oom_score_adj"};

    char filenames[length][20];

    for (int i = 0; i < length; i++) {
        sprintf(filenames[i], files[i], pi->pid);
        char* content = mem_parse_file(filenames[i], 8);
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


}

void read_process_mem_info(pid_t pid, struct sProcessMem* pm) {

}

void read_process_info(struct sProcessInfo* pi){
    const size_t length = 3;
    char* files[] = {"/proc/%d/oom_adj", "/proc/%d/oom_score", "/proc/%d/oom_score_adj"};

    char filenames[length][20];

    for (int i = 0; i < length; i++) {
        sprintf(filenames[i], files[i], pi->pid);
        char* content = mem_parse_file(filenames[i], 8);
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

    // read_process_mem_info(pi->pid, pi->memInfo);

}
