
#include "mem-writer.h"

#include <mem-info.h>

#include "process-reader.h"

#include "mem-threading.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FLUSH_INTERVAL 1

#define MASK_40 0xFFFFFFFFFF
#define MASK_32 0xFFFFFFFF
#define MASK_16 0xFFFF
#define MASK_12 0x0FFF
#define MASK_8 0xFF
#define MASK_6 0x3F
#define MASK_5 0x1F
#define MASK_4 0x0F

#define VERSION 3

#ifdef VERSION_1
#undef VERSION
#define VERSION 1
#endif


void writer_routine(struct mem_writer_s* mem_writer) {
    //    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if (mem_writer == NULL)
        return;

    while (1) {
        struct mtc_value_s* value = pop_from_mem_queue(mem_writer->writer_queue);
        if (value == NULL)
            continue;

        // If we purposely want to write 0 bytes we should exit
        if (value->length == 0) {
            break;
        }

#ifndef MEM_TEST
        const size_t bytes_written = fwrite((byte_t*) value->data, 1, value->length, mem_writer->file);
        // We have failed to write to the file
        if (bytes_written < 1) {
            perror("Error writing to file, writer routine exiting..");
            break;
        }

        // If FLUSH_INTERVAL is -1 let the OS decide when to flush
        if (mem_writer->flush_counter == FLUSH_INTERVAL) {
            fflush(mem_writer->file);
            mem_writer->flush_counter = 0;
        } else if (FLUSH_INTERVAL != -1) {
            mem_writer->flush_counter++;
        }
#else
        memcpy(mem_writer->file, value->data, value->length);
#endif

        free(value->data);
        free(value);
    }
#ifndef MEM_TEST
    fflush(mem_writer->file);
    fclose(mem_writer->file);
    mem_writer->file = NULL;
#endif
}


struct mem_writer_s* new_mem_writer() {
    struct mem_writer_s* mem_writer = malloc(sizeof(struct mem_writer_s));
    return mem_writer;
}

void init_mem_writer(struct mem_writer_s* mem_writer, char* filename) {
    mem_writer->filename = filename;
    mem_writer->file = fopen(mem_writer->filename, "ab");
    if (mem_writer->file == NULL) {
        exit(EXIT_FAILURE);
    }
    mem_writer->flush_counter = 0;

    MemQueue* writer_queue = malloc(sizeof(MemQueue));
    mem_queue_init(writer_queue);

    mem_writer->writer_queue = writer_queue;
    mem_writer->has_written_header = 0;
    mem_writer->prev_timestamp = NULL;

#ifndef MEM_TEST
    pthread_create(&mem_writer->pthread, NULL, (void* (*) (void*) ) writer_routine, mem_writer);
#endif
}

void destroy_mem_writer(struct mem_writer_s* mem_writer) {
    mem_queue_destroy(mem_writer->writer_queue);
#ifndef MEM_TEST
    pthread_cancel(mem_writer->pthread);
#endif
    if (mem_writer->file != NULL) {
        fclose(mem_writer->file);
    }

    free(mem_writer->writer_queue);
    free(mem_writer);
}


struct timeval* get_current_time() {
    struct timeval* tv = malloc(sizeof(struct timeval));
    gettimeofday(tv, NULL);

    return tv;
}

void* write_mtc_header(const struct timeval* tv, const uint8_t version) {
    const struct tm* local_time = localtime(&tv->tv_sec);

    const uint year = local_time->tm_year % 100 & MASK_6;
    const uint month = local_time->tm_mon & MASK_4;
    const uint day = local_time->tm_mday & MASK_5;
    const uint hour = local_time->tm_hour & MASK_5;
    const uint minute = local_time->tm_min & MASK_6;
    const uint second = local_time->tm_sec & MASK_6;

    byte_t* header = malloc(5);

    for (int i = 0; i < 5; i++) {
        header[i] = 0;
    }

    header[0] = version & MASK_8;

    header[1] |= year << 2;
    header[1] |= month >> 2;
    header[2] |= month << 6;
    header[2] |= day << 1;
    header[2] |= hour >> 4;
    header[3] |= hour << 4;
    header[3] |= minute >> 2;
    header[4] |= minute << 6;
    header[4] |= second;

    return header;
}

ushort timeval_diff_ms(const struct timeval* start_time, const struct timeval* end_time) {
    if (start_time == NULL || end_time == NULL) {
        return 0;
    }
    long long sec_diff = end_time->tv_sec - start_time->tv_sec;
    long long usec_diff = end_time->tv_usec - start_time->tv_usec;

    if (usec_diff < 0) {
        usec_diff += 1000000;
        sec_diff -= 1;
    }

    long long total_ms = (sec_diff * 1000) + (usec_diff / 1000);
    if (total_ms < 0)
        total_ms = 0;

    return total_ms;
}


void write_data_content(void* buffer, const uint offset, mk_size_t key, ushort value) {
    value &= MASK_16;
    key &= (byte_t) MASK_8;

    byte_t* dest = (byte_t*) buffer + offset;


    dest[0] = key;
#ifdef VERSION_1
    dest[1] = (byte_t) (value >> 8) & MASK_8;
    dest[2] = (byte_t) (value & MASK_8);
#else
    dest[1] = (byte_t) (value >> 16) & MASK_8;
    dest[2] = (byte_t) (value >> 8) & MASK_8;
    dest[3] = (byte_t) (value & MASK_8);
#endif
}

uint write_struct_data(void* buffer, void* struct_ptr, const uint struct_length, const uint mem_offset,
                       const uint key_offset) {
    uint writeOffset = 0;
    for (int i = 0; i < struct_length / SIZE_UL; i++) {
        const size_t value_offset = i * SIZE_UL;
        if (value_offset >= struct_length) {
            perror("Failed to read from struct!");
        }

        write_data_content(buffer, mem_offset + writeOffset, i + key_offset,
                           *(unsigned long*) ((byte_t*) struct_ptr + value_offset));
#ifdef VERSION_1
        writeOffset += 3;
#else
        writeOffset += 4;
#endif
    }

    return mem_offset + writeOffset;
}

void write_mem(struct mem_writer_s* mem_writer, MemInfo* mem_info, MemVmInfo* mem_vm_info, MemProcInfo* mem_proc_info) {
    if (mem_writer->has_written_header == 0) {
        mem_writer->prev_timestamp = get_current_time();

        void* header = write_mtc_header(mem_writer->prev_timestamp, mem_proc_info == NULL ? VERSION : VERSION + 1);
        add_to_mem_queue(mem_writer->writer_queue, header, 5);
        mem_writer->has_written_header = 1;
        return;
    }

    void* buffer = malloc(2048); // We really only need 769, apparently not..?

    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    struct timeval* tv = get_current_time();

    ushort milliseconds = timeval_diff_ms(mem_writer->prev_timestamp, tv) & MASK_16;

    byte_t* miliBuf = buffer;

    miliBuf[0] = (byte_t) (milliseconds >> 8 & MASK_8);
    miliBuf[1] = (byte_t) (milliseconds & MASK_8);

    free(mem_writer->prev_timestamp);
    mem_writer->prev_timestamp = tv;


    static const int starting_offset = 4;
    uint offset = starting_offset;

    offset = write_struct_data(buffer, mem_vm_info, sizeof(MemVmInfo), offset, 0);

    size_t value_length = sizeof(MemVmInfo) / SIZE_UL;
    offset = write_struct_data(buffer, mem_info, sizeof(MemInfo), offset, value_length);

    if (mem_proc_info != NULL) {
        value_length += sizeof(MemInfo) / SIZE_UL;
        offset = write_struct_data(buffer, mem_proc_info, sizeof(MemProcInfo), offset, value_length);
    }


    byte_t* countBuf = buffer + 2;
    const ushort value = offset - starting_offset;
    countBuf[0] = (byte_t) (value >> 8 & MASK_8);
    countBuf[1] = (byte_t) (value & MASK_8);

    add_to_mem_queue(mem_writer->writer_queue, buffer, offset);
}