#ifndef MEM_WRITER_H
#define MEM_WRITER_H


#include "mem-reader.h"

typedef struct sMemWriter MemWriter;


void init_mem_writer(MemWriter *mw, char* filename);

MemWriter* new_mem_writer();

void destroy_mem_writer(MemWriter *mw);

void write_mem(MemWriter *mw, struct sMemInfo* mi, struct sMemVmInfo* mp);



#endif //MEM_WRITER_H
