
#include "mem-info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE                                                                                                    \
    4096 // 4Kb. This should be more than enough for big systems,
         //        might need to tweek for lower systems..


void set_mem_struct_value(void* struct_ptr, const size_t struct_length, const char* map[], const char* key,
                          const unsigned long value) {
    for (int i = 0; i < struct_length / SIZE_UL; i++) {
        if (strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * SIZE_UL;
            if (valueOffset >= struct_length) {
                perror("Failed to write to struct!");
                return;
            }
            // This looks disgusting.. I thought I understood C but pointer arithmetic needs casting to 1 byte char?
            *(unsigned long*) ((char*) struct_ptr + valueOffset) = value;

            break;
        }
    }
}


char* mem_parse_file(const char* filename, size_t buffer_size, const uint8_t value) {
    char read_type[3];
    if (value == READ_RAW) {
        read_type[0] = 'r';
        read_type[1] = '\0';
    } else if (value == READ_BINARY) {
        read_type[0] = 'r';
        read_type[1] = 'b';
        read_type[2] = '\0';
    } else {
        perror("Read type invalid!");
    }

    FILE* fp = fopen(filename, read_type);
    if (fp == NULL) {
        return NULL;
    }

    size_t content_size = 0;
    if (buffer_size == -1) {
        buffer_size = BUFFER_SIZE;
    }

    char* content = malloc(buffer_size);


    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    size_t bytes_read;
    while ((bytes_read = fread(content + content_size, 1, buffer_size - content_size - 1, fp)) > 0) {
        content_size += bytes_read;
    }
    content[content_size] = '\0';

    fclose(fp);

    if (content_size == 0) return NULL;
    return content;
}
