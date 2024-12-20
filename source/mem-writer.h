#ifndef MEM_WRITER_H
#define MEM_WRITER_H


#include "mem-reader.h"
#include "process-reader.h"

#include <sys/time.h>

struct sMemWriter {
    char* filename;
    FILE* file;
    unsigned char flushCounter;
    int hasWrittenHeader;
    struct timeval* prevTimestamp;


    struct mem_queue* writer_queue;


    pthread_t pthread;
};

typedef struct sMemWriter MemWriter;

typedef unsigned int uint;
typedef unsigned long long uint64;
typedef unsigned short ushort;

void writer_routine(struct sMemWriter* mw);

void init_mem_writer(MemWriter *mw, char* filename);

MemWriter* new_mem_writer();

void destroy_mem_writer(MemWriter *mw);

void write_mem(MemWriter *mw, struct sMemInfo* mi, struct sMemVmInfo* mp, struct sMemProcessInfo* pi);

struct timeval* get_current_time();

ushort timeval_diff_ms(struct timeval* start, struct timeval* end);

void* write_mtc_header(struct timeval* tv);

uint write_struct_data(void* buffer, void* sStruct, uint structLength, uint mem_offset, uint key_offset);

void write_data_content(void* buffer, uint offset, unsigned char key, ushort value);

#endif //MEM_WRITER_H
