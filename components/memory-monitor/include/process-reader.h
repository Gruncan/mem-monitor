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

typedef struct proc_info {
    pid_t pid;
    MemProcInfo* mem_info;
    char is_alive;
} ProcInfo;

typedef struct process_ids_s {
    ProcInfo* proc_info;
    size_t size;
    char* name;
    unsigned char is_proc_override;
} ProcessIds;



ProcessIds* get_pids_by_name(char* name, unsigned char is_proc_override);

void init_process_ids(ProcessIds* process_id, const pid_t* pids, size_t size, char* name, unsigned char is_proc_override);

void check_processes_exists(const ProcessIds* process_id);

int init_process_info(MemProcInfo* mem_proc_info);

void read_process_mem_info(MemProcInfo* mem_proc_info, pid_t pid);

char read_single_process_info(MemProcInfo* mem_proc_info, pid_t pid);

void reset_process_info(MemProcInfo* mem_proc_info);

unsigned char read_processes(ProcessIds* processes);

#endif // PROCESS_READER_H
