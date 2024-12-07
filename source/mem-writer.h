#ifndef MEM_WRITER_H
#define MEM_WRITER_H


#include "mem-reader.h"
#include "process-reader.h"

typedef struct sMemWriter MemWriter;

typedef unsigned int uint;
typedef unsigned long long uint64;
typedef unsigned short ushort;


void init_mem_writer(MemWriter *mw, char* filename);

MemWriter* new_mem_writer();

void destroy_mem_writer(MemWriter *mw);

void write_mem(MemWriter *mw, struct sMemInfo* mi, struct sMemVmInfo* mp, struct sProcessInfo* pi);

int write_struct_data(void* buffer, void* sStruct, uint structLength, uint offset);

void write_data_content(void* buffer, uint offset, char key, ushort value);

#endif //MEM_WRITER_H
