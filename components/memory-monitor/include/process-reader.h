/***********************************
 *
 * @brief Process specific reading struct and signatures
 *
 * @file process-reader.h
 *
************************************/
#ifndef PROCESS_READER_H
#define PROCESS_READER_H

#include <sys/types.h>


typedef struct mem_proc_info_s {
    long oom_adj;
    long oom_score;
    long oom_score_adj;
    unsigned long size;
    unsigned long resident;
    unsigned long shared;
    unsigned long text;
    unsigned long data;
    unsigned long dirty;
} MemProcInfo;


int check_process_exists(pid_t pid);

int init_process_info(MemProcInfo* mem_proc_info, pid_t pid);

void read_process_mem_info(MemProcInfo* mem_proc_info, pid_t pid);

char read_process_info(MemProcInfo* mem_proc_info, pid_t pid);

void reset_process_info(MemProcInfo* mem_proc_info);


#endif // PROCESS_READER_H
