#ifndef PROCESS_READER_H
#define PROCESS_READER_H

#include <sys/types.h>


struct sMemProcessInfo {
    long oomAdj;
    long oomScore;
    long oomScoreAdj;
    unsigned long size;
    unsigned long resident;
    unsigned long shared;
    unsigned long text;
    unsigned long data;
    unsigned long dirty;
};


void init_process_info(struct sMemProcessInfo* pi);

void read_process_info(struct sMemProcessInfo* pi, pid_t pid);

void read_process_mem_info(struct sMemProcessInfo* pi, pid_t pid);

void reset_process_info(struct sMemProcessInfo* pi);


#endif //PROCESS_INFO_H
