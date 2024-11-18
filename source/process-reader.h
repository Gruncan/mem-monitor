#ifndef PROCESS_READER_H
#define PROCESS_READER_H

#include <sys/types.h>

struct sProcessMem {
    unsigned long size;
    unsigned long resident;
    unsigned long shared;
    unsigned long text;
    unsigned long data;
    unsigned long dirty;
};

struct sProcessMemStat {
    unsigned long vsize;
    unsigned long minor_faults;
    unsigned long major_faults;
};

struct sProcessInfo {
    pid_t pid;
    char* name;
    int oomAdj;
    int oomScore;
    int oomScoreAdj;
    struct sProcessMem* memInfo;
    struct sProcessMemStat* memStat;
};


//void process_reader


void init_process_info(struct sProcessInfo* info, pid_t pid);

void read_process_info(struct sProcessInfo* info);

void read_process_mem_info(pid_t pid, struct sProcessMem* info, struct sProcessMemStat* pms);


void reset_process_info(struct sProcessInfo* info);

struct memInfoStrings* get_process_mem_info_names(struct sProcessMem* pm);

const char** get_process_mem_names();

struct memInfoStrings* get_process_mem_stat_info_names(struct sProcessMemStat* pms);

const char** get_process_mem_stat_names();


#endif //PROCESS_INFO_H
