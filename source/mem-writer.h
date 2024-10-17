#ifndef MEM_WRITER_H
#define MEM_WRITER_H


#include "mem-info.h"

struct sMemWriter {
    char* filename;
};


void init_mem_writer(struct sMemWriter *mw, char* filename);

void write_mem(const struct sMemWriter *mw, struct sMemInfo* mi);



#endif //MEM_WRITER_H
