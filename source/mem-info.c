
#include "mem-info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096  // 4Kb. This should be more than enough for big systems,
                          //        might need to tweek for lower systems..


static const size_t sizeUL = sizeof(unsigned long);



void set_mem_struct_value(void* sStruct, const size_t structLength,
                          const char* map[], const char* key, const unsigned long value) {
    for (int i = 0; i < structLength / sizeUL; i++) {
        if(strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * sizeUL;
            if (valueOffset >= structLength) {
                perror("Failed to write to struct!");
                return;
            }
            // This looks disgusting.. I thought I understood C but pointer arithmetic needs casting to 1 byte char?
            *(unsigned long*)((char*)sStruct + valueOffset) = value;

            break;
        }
    }
}

unsigned long get_mem_struct_value(void* sStruct, const size_t structLength, const char* map[], const char* key) {
    for (int i = 0; i < structLength / sizeUL; i++) {
        if (strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * sizeUL;
            if (valueOffset >= structLength) {
                perror("Failed to read from struct!");
            }
            return *(unsigned long*) ((char*) sStruct + valueOffset);
        }
    }

    return -1; // Error memory stat should not be possible for negative?
}


char* mem_parse_file(const char* filename, size_t bufferSize, uint8_t value) {
    char read_type[3];
    if (value == READ_RAW) {
        read_type[0]= 'r'; read_type[1] = '\0';
    }else if (value == READ_BINARY) {
        read_type[0] = 'r'; read_type[1] = 'b'; read_type[2] = '\0';
    }else {
        perror("Read type invalid!");
    }

    FILE *fp = fopen(filename, read_type);
    if (fp == NULL) {
        return NULL;
    }

    size_t content_size = 0;
    if(bufferSize == -1) {
        bufferSize = BUFFER_SIZE;
    }

    char* content = malloc(bufferSize);


    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    size_t bytes_read;
    while ((bytes_read = fread(content + content_size, 1, bufferSize - content_size - 1, fp)) > 0) {
        content_size += bytes_read;
    }
    content[content_size] = '\0';

    fclose(fp);

    return content;
}


