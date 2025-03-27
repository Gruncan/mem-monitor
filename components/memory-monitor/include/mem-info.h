/***********************************
 *
 * @brief Generic functions for reading a file
 *
 * @file mem-info.h
 *
************************************/
#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stddef.h>
#include <stdint.h>

#define READ_RAW 0
#define READ_BINARY 1

const static size_t SIZE_UL = sizeof(unsigned long);

/**
 * Sets a memory structs values based of value name, struct is assumed to contain only unsigned longs
 */
void set_mem_struct_value(void* struct_ptr, size_t struct_length, const char* map[], const char* key,
                          unsigned long value);


/**
 * Loads a file into a buffer of specified size
 * @param filename the file to load
 * @param buffer_size the size to read into
 * @param value the file handler flag either READ_RAW or READ_BINARY
 * @return
 */
char* mem_parse_file(const char* filename, size_t buffer_size, uint8_t value);


#endif // MEM_INFO_H
