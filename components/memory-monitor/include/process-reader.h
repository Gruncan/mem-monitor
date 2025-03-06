#ifndef PROCESS_READER_H
#define PROCESS_READER_H

#include <stdint.h>
#include <sys/types.h>


#define SMAP_VERSION



typedef struct mem_proc_smap_rollup_s {
    unsigned long pid;
    unsigned long rss;
    unsigned long pss;
    unsigned long pss_dirty;
    unsigned long pss_anon;
    unsigned long pss_file;
    unsigned long pss_shmem;
    unsigned long shared_clean;
    unsigned long shared_dirty;
    unsigned long private_clean;
    unsigned long private_dirty;
    unsigned long referenced;
    unsigned long anonymous;
    unsigned long ksm;
    unsigned long lazy_free;
    unsigned long anon_huge_pages;
    unsigned long shmem_pmd_mapped;
    unsigned long file_pmd_mapped;
    unsigned long shared_hugetld;
    unsigned long private_hugetlb;
    unsigned long swap;
    unsigned long swap_pss;
    unsigned long locked;

} MemProcSmapRollup;

typedef struct mem_proc_info_s {
    unsigned long pid;
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

#ifdef SMAP_VERSION
#define MemProc MemProcSmapRollup
#define MemProcFields 23
#define read_proc_info(...) read_process_mem_smap_rollup_info(##__VA_ARGS__)
#define reset_proc_info(...) reset_process_smap_info(##__VA_ARGS__)
#define MAX_PROCESS 8
#else
#define MemProc MemProcInfo
#define MemProcFields 10
#define read_proc_info(...) read_single_process_info(##__VA_ARGS__)
#define reset_proc_info(...) reset_process_mem_info(##__VA_ARGS__)
#define MAX_PROCESS 16
#endif



typedef struct proc_info {
    pid_t pid;
    MemProc* mem_info;
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

int init_process_info(MemProc* mem_proc_info, pid_t pid);

void read_process_mem_info(MemProcInfo* mem_proc_info, pid_t pid);

char read_single_process_info(MemProcInfo* mem_proc_info, pid_t pid);

char read_process_mem_smap_rollup_info(MemProcSmapRollup* mem_smap, const pid_t pid);

void reset_process_mem_info(MemProcInfo* mem_proc_info);

void reset_process_smap_info(MemProcSmapRollup* mem_smap);

unsigned char read_processes(ProcessIds* processes);

#endif // PROCESS_READER_H
