
#include "mem-writer.h"

#include <mem-info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void init_mem_writer(struct sMemWriter *mw, char* filename) {
    mw->filename = filename;
}


char* get_current_time() {
    const time_t current_time = time(NULL);

    if (current_time == -1) {
        perror("Error getting the current time");
        return NULL;
    }

    struct tm *local_time = localtime(&current_time);

    if (local_time == NULL) {
        perror("Error converting to local time");
        return NULL;
    }

    char* time_string = malloc(100);

    strftime(time_string, 100, "%Y-%m-%d %H:%M:%S", local_time);

    return time_string;
}

void write_mem(const struct sMemWriter *mw, struct sMemInfo* mi){
    FILE *fp = fopen(mw->filename, "a");
    if (fp == NULL) {
        return;
    }


    char* buffer = malloc(2048);

    if (buffer == NULL) {
        perror("Error allocating memory");
        return;
    }

    buffer[0] = '\0';



    char* time_string = get_current_time();

    char** mem_data = get_all_mem_info_data(mi);


    // fprintf(fp, "{\"%s\":{\"%s\"}}", time_string);


    destroy_all_mem_data(mem_data);
    free(time_string);
    free(buffer);
}