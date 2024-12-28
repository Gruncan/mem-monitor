#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stddef.h>
#include <stdint.h>

#define READ_RAW 0
#define READ_BINARY 1


void set_mem_struct_value(void* sStruct, size_t structLength, const char* map[], const char* key, unsigned long value);


char* mem_parse_file(const char* filename, size_t bufferSize, uint8_t value);


#endif // MEM_INFO_H
