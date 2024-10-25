#ifndef PROCESS_READER_H
#define PROCESS_READER_H

#include <sys/types.h>

struct sProcessMem {
  unsigned long size;
  unsigned long resident;
  unsigned long share;
  unsigned long text;
  unsigned long data;
  unsigned long dirty;

};

struct sProcessInfo {
  pid_t pid;
  char* name;
  int oomAdj;
  int oomScore;
  int oomScoreAdj;
  struct sProcessMem* memInfo;
};


//void process_reader


void init_process_info(struct sProcessInfo* info, pid_t pid);

void read_process_info(struct sProcessInfo* info);

void read_process_mem_info(pid_t pid, struct sProcessMem* info);




#endif //PROCESS_INFO_H
