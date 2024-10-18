#ifndef MEM_WRITER_H
#define MEM_WRITER_H


#include <stdio.h>

#include "mem-info.h"

struct sMemWriter {
    char* filename;
    FILE* file;
    unsigned char flushCounter;
};


void init_mem_writer(struct sMemWriter *mw, char* filename);

void destroy_mem_writer(struct sMemWriter *mw);

void write_mem(struct sMemWriter *mw, struct sMemInfo* mi);



#endif //MEM_WRITER_H
