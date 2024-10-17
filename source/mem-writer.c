
#include "mem-writer.h"

#include <mem-info.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


void init_mem_writer(struct sMemWriter *mw, char* filename) {
    mw->filename = filename;
}


char* get_current_time() {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    const struct tm *tm_info = localtime(&tv.tv_sec);

    char* time_string = malloc(200);
    time_string[0] = '\0';

    strftime(time_string, 200, "%Y-%m-%d %H:%M:%S", tm_info);

    char tmp[10];
    snprintf(tmp, sizeof(tmp), ".%03ld", tv.tv_usec / 1000);
    strcat(time_string,  tmp);

    return time_string;
}

void write_mem(const struct sMemWriter *mw, struct sMemInfo* mi){
    FILE *fp = fopen(mw->filename, "a");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }


    char* buffer = malloc(2048);

    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    buffer[0] = '\0';

    char* time_string = get_current_time();

    struct memInfoStrings* mem_data = get_all_mem_info_data(mi);

    const char** mem_names = get_mem_info_names();

    char temp[50];
    snprintf(temp, sizeof(temp), "{\"%s\":{", time_string);
    strcat(buffer, temp);

    for (int i = 0; i < mem_data->mem_strings_count; i++) {
        size_t value_len = strlen(mem_data->mem_strings[i]);
        size_t name_len = strlen(mem_names[i]);
        char tmp[value_len + name_len + 10];


        snprintf(tmp, sizeof(tmp), "\"%s\": \"%s\"", mem_names[i], mem_data->mem_strings[i]);
        strcat(buffer,  tmp);

        if (i < mem_data->mem_strings_count - 1) {
            strcat(buffer, ", ");
        }
    }

    strcat(buffer, "}}\n");

    size_t bytes_written = fwrite(buffer, sizeof(char), strlen(buffer), fp);

    if (bytes_written < 1) {
        perror("Error writing to file");
        exit(EXIT_FAILURE);
    }

    // TODO improve this so we dont flush on ever write!
    fflush(fp);

    free(fp);

    destroy_all_mem_data(mem_data);
    free(time_string);
    free(buffer);
}