#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stddef.h>


struct memInfoStrings {
    char** mem_strings;
    size_t mem_strings_count;
};

char* get_ulong_str(unsigned long ulong_value);

void set_mem_struct_value(void* sStruct, size_t structLength,
                          const char* map[], const char* key, unsigned long value);

unsigned long get_mem_struct_value(void* sStruct, size_t structLength, const char* map[], const char* key);

struct memInfoStrings* get_all_mem_struct_values(const unsigned long* values, size_t valuesLength);


char* mem_parse_file(const char* filename, size_t bufferSize);


#endif //MEM_INFO_H
