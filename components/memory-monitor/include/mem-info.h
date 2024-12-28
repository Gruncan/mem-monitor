#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stddef.h>
#include <stdint.h>

#define READ_RAW 0
#define READ_BINARY 1

const static size_t SIZE_UL = sizeof(unsigned long);


void set_mem_struct_value(void* struct_ptr, size_t struct_length, const char* map[], const char* key,
                          unsigned long value);


char* mem_parse_file(const char* filename, size_t buffer_size, uint8_t value);


#endif // MEM_INFO_H
