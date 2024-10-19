
#include "mem-writer.h"
#include "mem-info.h"
#include "mem-threading.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>


#define FLUSH_INTERVAL 1

struct sMemWriter {
    char* filename;
    FILE* file;
    unsigned char flushCounter;

    struct mem_writer_queue* writer_queue;

    pthread_t pthread;
};



void writer_routine(struct sMemWriter* mw) {
    if (mw == NULL) return;

    while(1) {
        char* buffer = pop_from_mem_writer_queue(mw->writer_queue);
        if (buffer == NULL) continue;

        const size_t bytes_written = fwrite(buffer, sizeof(char), strlen(buffer), mw->file);
        if (bytes_written < 1) {
            perror("Error writing to file, writer routine exiting..");
            break;
        }

        // printf("Wrote %zu bytes\n", bytes_written);

        // If FLUSH_INTERVAL is -1 let the OS decide when to flush
        if (mw->flushCounter == FLUSH_INTERVAL) {
            fflush(mw->file);
            mw->flushCounter = 0;
        }else if(FLUSH_INTERVAL != -1) {
            mw->flushCounter++;
        }

        free(buffer);

    }
}


struct sMemWriter* new_mem_writer() {
    struct sMemWriter* mw = malloc(sizeof(struct sMemWriter));
    return mw;
}

void init_mem_writer(struct sMemWriter *mw, char* filename) {
    mw->filename = filename;
    mw->file = fopen(mw->filename, "a");
    if (mw->file  == NULL) {
        exit(EXIT_FAILURE);
    }
    mw->flushCounter = 0;

    struct mem_writer_queue* writer_queue = malloc(sizeof(struct mem_writer_queue));
    mem_writer_queue_init(writer_queue);

    mw->writer_queue = writer_queue;


    pthread_create(&mw->pthread, NULL, writer_routine, mw);

}

void destroy_mem_writer(struct sMemWriter *mw) {

    mem_writer_queue_destroy(mw->writer_queue);
    pthread_kill(mw->pthread, SIGKILL);

    if (mw->file != NULL) {
        fclose(mw->file);
    }

    free(mw->writer_queue);
    free(mw);
}


char* get_current_time() {
    struct timeval tv;

    gettimeofday(&tv, NULL);

    const struct tm *tm_info = localtime(&tv.tv_sec);

    char* time_string = malloc(200);
    time_string[0] = '\0';

    strftime(time_string, 200, "%Y-%m-%d %H:%M:%S", tm_info);

    char tmp[20];
    snprintf(tmp, sizeof(tmp), ".%03ld", tv.tv_usec / 1000);
    strcat(time_string,  tmp);

    return time_string;
}

void write_mem(struct sMemWriter *mw, struct sMemInfo* mi){

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


    add_to_mem_writer_queue(mw->writer_queue, buffer);


    destroy_all_mem_data(mem_data);
    free(time_string);
}