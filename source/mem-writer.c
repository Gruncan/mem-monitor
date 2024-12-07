
#include "mem-writer.h"

#include "process-reader.h"

#include "mem-info.h"
#include "mem-threading.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

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



void writer_routine(struct sMemWriter* mw) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if (mw == NULL) return;

    while(1) {
        struct mtc_value* value = pop_from_mem_queue(mw->writer_queue);
        if (value == NULL) continue;

        // If we purposely want to write 0 bytes we should exit
        if (value->length == 0){
            break;
        }

#ifndef MEM_TEST
        const size_t bytes_written = fwrite((unsigned char*) value->data, 1, value->length, mw->file);
        // We have failed to write to the file
        if (bytes_written < 1) {
            perror("Error writing to file, writer routine exiting..");
            break;
        }

        // If FLUSH_INTERVAL is -1 let the OS decide when to flush
        if (mw->flushCounter == FLUSH_INTERVAL) {
            fflush(mw->file);
            mw->flushCounter = 0;
        }else if(FLUSH_INTERVAL != -1) {
            mw->flushCounter++;
        }
#else
        memcpy(mw->file, value->data, value->length);
#endif

        free(value->data);
        free(value);
    }
#ifndef MEM_TEST
    fflush(mw->file);
    fclose(mw->file);
    mw->file = NULL;
#endif
}


struct sMemWriter* new_mem_writer() {
    struct sMemWriter* mw = malloc(sizeof(struct sMemWriter));
    return mw;
}

void init_mem_writer(struct sMemWriter* mw, char* filename) {
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


    pthread_create(&mw->pthread, NULL, (void*(*)(void*)) writer_routine, mw);

}

void destroy_mem_writer(struct sMemWriter* mw) {

    mem_queue_destroy(mw->writer_queue);
    pthread_cancel(mw->pthread);

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

    return header;
}

short timeval_diff_ms(struct timeval* start, struct timeval* end) {
    long long sec_diff = end->tv_sec - start->tv_sec;
    long long usec_diff = end->tv_usec - start->tv_usec;

    if (usec_diff < 0) {
        usec_diff += 1000000;
        sec_diff -= 1;
    }

    long long total_ms = (sec_diff * 1000) + (usec_diff / 1000);
    if (total_ms < 0) total_ms = 0;

    return (short) total_ms;
}




void write_data_content(void* buffer, uint offset, char key, ushort value){
    value &= MASK_16;
    key &= (char) MASK_8;

    char* dest = (char*)buffer + offset;

    dest[0] = key;
    dest[1] = (char) (value >> 8) & MASK_8;
    dest[2] = (char) (value & MASK_8);
}

int write_struct_data(void* buffer, void* sStruct, uint structLength, uint offset) {
    static const uint sizeUL = sizeof(unsigned long);
    uint writeOffset = 0;
    for (int i = 0; i < structLength / sizeUL; i++) {
        const size_t valueOffset = i * sizeUL;
        if (valueOffset >= structLength) {
            perror("Failed to read from struct!");
        }
        // We offset by an additional 1 since
        write_data_content(buffer, offset + writeOffset, (char) i + offset,
                                    *(unsigned long*) ((char*) sStruct + valueOffset));
        writeOffset += 3;
    }

    return offset + writeOffset;
}

void write_mem(struct sMemWriter *mw, struct sMemInfo* mi, struct sMemVmInfo* mp, struct sProcessInfo* pi) {

    if (mw->hasWrittenHeader == 0) {
        mw->prevTimestamp = get_current_time();
        void* header = write_mtc_header(mw->prevTimestamp);
        add_to_mem_queue(mw->writer_queue, header, 5);
        mw->hasWrittenHeader = 1;
        return;
    }

    void* buffer = malloc(2048); // We really only need 769, apparently not..?

    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    struct timeval* tv = get_current_time();

    short miliseconds = timeval_diff_ms(tv, mw->prevTimestamp) & MASK_16;

    char* miliBuf = buffer;
    miliBuf[0] = (char)(miliseconds >> 8 & 0xFF);
    miliBuf[1] = (char)(miliseconds & 0xFF);

    free(mw->prevTimestamp);
    mw->prevTimestamp = tv;


    uint offset = 5;

    offset = write_struct_data(buffer, mp, sizeof(struct sMemVmInfo), offset);

    offset = write_struct_data(buffer, mi, sizeof(struct sMemInfo), offset+1);


    if (pi != NULL) {
        offset = write_struct_data(buffer, pi, sizeof(struct sProcessInfo*), offset+1);
    }


    char* contBuf = buffer + 3;
    const ushort value = (offset - 4);
    contBuf[0] = (char)(value >> 8 & 0xFF);
    contBuf[1] = (char)(value & 0xFF);



    add_to_mem_queue(mw->writer_queue, buffer, offset);
}