
#include "mem-writer.h"

#include <process-reader.h>

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

#define MASK_40 0xFFFFFFFFFF
#define MASK_32 0xFFFFFFFF
#define MASK_16 0xFFFF
#define MASK_12 0x0FFF
#define MASK_8  0x00FF
#define MASK_6  0x003F
#define MASK_5  0x001F
#define MASK_4  0x000F



#define VERSION 1


typedef unsigned int uint;
typedef unsigned long long uint64;

struct sMemWriter {
    char* filename;
    FILE* file;
    unsigned char flushCounter;
    int hasWrittenHeader;
    struct timeval* prevTimestamp;


    struct mem_queue* writer_queue;


    pthread_t pthread;
};



void writer_routine(struct sMemWriter* mw) {
    if (mw == NULL) return;

    while(1) {
        struct mtc_value* value = pop_from_mem_queue(mw->writer_queue);
        if (value == NULL) continue;

        const size_t bytes_written = fwrite(value->data, 1, value->length, mw->file);
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

        free(value->data);
        free(value);

    }
}


struct sMemWriter* new_mem_writer() {
    struct sMemWriter* mw = malloc(sizeof(struct sMemWriter));
    return mw;
}

void init_mem_writer(struct sMemWriter *mw, char* filename) {
    mw->filename = filename;
    mw->file = fopen(mw->filename, "ab");
    if (mw->file  == NULL) {
        exit(EXIT_FAILURE);
    }
    mw->flushCounter = 0;

    struct mem_queue* writer_queue = malloc(sizeof(struct mem_queue));
    mem_queue_init(writer_queue);

    mw->writer_queue = writer_queue;
    mw->hasWrittenHeader = 0;
    mw->prevTimestamp = NULL;


    pthread_create(&mw->pthread, NULL, writer_routine, mw);

}

void destroy_mem_writer(struct sMemWriter *mw) {

    mem_queue_destroy(mw->writer_queue);
    pthread_kill(mw->pthread, SIGKILL);

    if (mw->file != NULL) {
        fclose(mw->file);
    }

    free(mw->writer_queue);
    free(mw);
}


struct timeval* get_current_time() {
    struct timeval* tv = malloc(sizeof(struct timeval));

    gettimeofday(tv, NULL);

    return tv;
}

void* write_mtc_header(struct timeval* tv) {
    struct tm* local_time = localtime(&tv->tv_sec);

    uint year = (local_time->tm_year % 100) & MASK_6;
    uint month = local_time->tm_mon & MASK_4;
    uint day = local_time->tm_mday & MASK_5;
    uint hour = local_time->tm_hour & MASK_5;
    uint minute = local_time->tm_min & MASK_6;
    uint second = local_time->tm_sec & MASK_6;


    uint8_t* header = malloc(5);

    for (int i = 0; i < 5; i++) {
        header[i] = 0;
    }

    header[0] = VERSION;

    header[1] |= (year << 2);
    header[1] |= (month >> 2);
    header[2] |= (month << 6);
    header[2] |= (day << 1);
    header[2] |= (hour >> 4);
    header[3] |= (hour << 4);
    header[3] |= (minute >> 2);
    header[4] |= (minute << 6);
    header[4] |= second;

    printf("Bits in header:\n");
    for (int i = 0; i < 5; i++) {
        printf("Byte %d: ", i);
        for (int bit = 7; bit >= 0; bit--) { // Print each bit from MSB to LSB
            printf("%d", (header[i] >> bit) & 1);
        }
        printf("\n");
    }

    return header;
}

int timeval_diff_ms(struct timeval* start, struct timeval* end) {
    long long sec_diff = end->tv_sec - start->tv_sec;
    long long usec_diff = end->tv_usec - start->tv_usec;

    if (usec_diff < 0) {
        usec_diff += 1000000;
        sec_diff -= 1;
    }

    long long total_ms = (sec_diff * 1000) + (usec_diff / 1000);

    return ((int) total_ms) & MASK_12;
}




int write_data_content(void* buffer, uint offset, uint key, unsigned long value) {
    uint bytes = 0;
    bytes |= value & MASK_16;
    bytes |= (key & MASK_8) << 16;

    uint8_t* dest = (uint8_t*)buffer + offset;

    dest[0] = (uint8_t)(bytes & MASK_8);
    dest[1] = (uint8_t)((bytes >> 8) & MASK_8);
    dest[2] = (uint8_t)((bytes >> 16) & MASK_8);
}

int write_struct_data(void* buffer, void* sStruct, uint structLength, uint offset) {
    static const uint sizeUL = sizeof(unsigned long);
    for (int i = 0; i < structLength / sizeUL; i++) {
        const size_t valueOffset = i * sizeUL;
        if (valueOffset >= structLength) {
            perror("Failed to read from struct!");
        }
        write_data_content(buffer, offset + valueOffset, valueOffset,
                                    *(unsigned long*) ((char*) sStruct + valueOffset));
    }

    return offset + structLength;
}

void write_mem(struct sMemWriter *mw, struct sMemInfo* mi, struct sMemVmInfo* mp, struct sProcessInfo* pi) {
    void* buffer = malloc(2048); // We really only need 769, apparently not..?

    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    if (mw->hasWrittenHeader == 0) {
        mw->prevTimestamp = get_current_time();
        void* header = write_mtc_header(mw->prevTimestamp);
        add_to_mem_queue(mw->writer_queue, header, 5);
        mw->hasWrittenHeader = 1;
        return;
    }

    struct timeval* tv = get_current_time();

    int miliseconds = timeval_diff_ms(tv, mw->prevTimestamp) & MASK_16;

    uint8_t* miliBuf = buffer;
    miliBuf[0] = (uint8_t)(miliseconds >> 8);
    miliBuf[1] = (uint8_t)(miliseconds << 8);


    free(mw->prevTimestamp);
    mw->prevTimestamp = tv;


    uint offset = 5;

    offset = write_struct_data(buffer, mp, sizeof(struct sMemVmInfo), offset);

    offset = write_struct_data(buffer, mi, sizeof(struct sMemInfo), offset);


    if (pi != NULL) {
        offset = write_struct_data(buffer, pi, sizeof(struct sProcessInfo*), offset);
    }


    uint8_t* contBuf = buffer + 3;
    const uint value = (offset - 4) & MASK_16;
    contBuf[0] = (uint8_t)(value >> 8);
    contBuf[1] = (uint8_t)(value << 8);



    add_to_mem_queue(mw->writer_queue, buffer, offset);
}