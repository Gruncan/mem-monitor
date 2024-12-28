#ifndef MEM_WRITER_H
#define MEM_WRITER_H


#include "mem-reader.h"
#include "mem-threading.h"
#include "process-reader.h"

#include <sys/time.h>

typedef struct mem_writer_s {
    char* filename;
    FILE* file;
    unsigned char flush_counter;
    int has_written_header;
    struct timeval* prev_timestamp;


    MemQueue* writer_queue;


    pthread_t pthread;
} MemWriter;


typedef unsigned int uint;
typedef unsigned long long uint64;
typedef unsigned short ushort;

void writer_routine(MemWriter* mem_writer);

void init_mem_writer(MemWriter* mem_writer, char* filename);

MemWriter* new_mem_writer();

void destroy_mem_writer(MemWriter* mem_writer);

void write_mem(MemWriter* mem_writer, MemInfo* mem_info, MemVmInfo* mem_vm_info, MemProcInfo* mem_proc_info);

struct timeval* get_current_time();

ushort timeval_diff_ms(const struct timeval* start_time, const struct timeval* end_time);

void* write_mtc_header(struct timeval* tv);

uint write_struct_data(void* buffer, void* struct_ptr, uint struct_length, uint mem_offset, uint key_offset);

void write_data_content(void* buffer, uint offset, unsigned char key, ushort value);

#endif // MEM_WRITER_H
