
#include "mem-writer.h"
#include "mem-threading.h"
#include "mem-reader.h"
#include "process-reader.h"

#include "test.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#define STRUCT_WRITE_SIZE(s) ((sizeof(s) / SIZE_UL) * 3)


int test_struct_writer();
int test_data_writer1();
int test_data_writer2();
int test_data_writer3();
int test_timeval_diff_ms();
int test_writer_routine();
int test_create_destroy_mem_writer();
int test_write_mtc_header();
int test_write_mem_header();
int test_write_mem_body_1();
int test_write_mem_body_2();

static const uint SIZE_UL = sizeof(unsigned long);


int main(int argc, char *argv[]){
    INIT_TEST

    TEST(test_struct_writer)

    TEST(test_data_writer1)
    TEST(test_data_writer2)
    TEST(test_data_writer3)

    TEST(test_timeval_diff_ms)

    TEST(test_writer_routine)

    TEST(test_create_destroy_mem_writer)

    TEST(test_write_mtc_header)
    TEST(test_write_mem_header)

    TEST(test_write_mem_body_1)


    PRINT_RESULTS

    return EXIT_VALUE;
}


struct testStruct {
    unsigned long v1;
    unsigned long v2;
    unsigned long v3;
    unsigned long v4;
};

int test_struct_writer() {
    struct testStruct t1 = {1, 2, 3, 4};

    size_t length = sizeof(struct testStruct) / sizeof(unsigned long);
    size_t size = sizeof(struct testStruct);

    char* buffer = malloc(length + 4);


    int offset = write_struct_data(buffer, &t1, size, 0, 0);
    ASSERT_EQUAL(offset, (length * 2) + 4);

    int v = 0;
    for (int i = 0; i < length + 4; i += 3) {
        ASSERT_EQUAL(buffer[i], v); // Key
        short value = buffer[i + 1] << 8 | buffer[i + 2];
        ASSERT_EQUAL(value, v+1);
        v++;
    }

    free(buffer);

    return PASS;
}

int test_data_writer1(){

    char* buffer = malloc(3);

    memset(buffer, 0, 3);

    write_data_content(buffer, 0, 1, 4);

    ASSERT_EQUAL(buffer[0], 1);
    ASSERT_EQUAL(buffer[1], 0);
    ASSERT_EQUAL(buffer[2], 4);

    return PASS;
}

int test_data_writer2(){
    char* buffer = malloc(6);

    memset(buffer, 0, 6);


    write_data_content(buffer, 3, 1, 4);

    ASSERT_EQUAL(buffer[3], 1);
    ASSERT_EQUAL(buffer[4], 0);
    ASSERT_EQUAL(buffer[5], 4);

    return PASS;
}

int test_data_writer3(){
    char* buffer = malloc(3);

    memset(buffer, 0, 3);

    write_data_content(buffer, 0, 1, 65000);

    ASSERT_EQUAL(buffer[0], 1);
    ASSERT_EQUAL((unsigned char)buffer[1], 253);
    ASSERT_EQUAL((unsigned char)buffer[2], 232)

    return PASS;
}

int test_timeval_diff_ms(){

    struct timeval time_1 = {1629387587, 500000};
    struct timeval time_2 = {1629387587, 510000};

    short milliseconds1 = timeval_diff_ms(&time_1, &time_2);

    ASSERT_EQUAL(milliseconds1, 10);

    short milliseconds2 = timeval_diff_ms(&time_2, &time_1);

    ASSERT_EQUAL(milliseconds2, 0)

    return PASS;
}

struct mem_value* init_test_mem_value(struct mem_value* next, void* data, unsigned int length){
    struct mem_value* test_value = malloc(sizeof(struct mem_value));
    test_value->next = next;

    struct mtc_value* test_mtc_value = malloc(sizeof(struct mtc_value));
    test_mtc_value->data = data;
    test_mtc_value->length = length;

    test_value->value = test_mtc_value;

    return test_value;
}


int test_writer_routine(){
    struct mem_queue test_queue;

    mem_queue_init(&test_queue);

    struct sMemWriter test_writer = {"test", NULL, 0, 0, NULL, &test_queue, 0};

    void* buffer = malloc(5);

    test_writer.file = buffer;

    struct mem_value* tailValue = init_test_mem_value(NULL, NULL, 0);

    unsigned char* values = malloc(5);

    for (int i = 0; i < 5; i++){
        values[i] = i+1;
    }

    struct mem_value* headValue = init_test_mem_value(tailValue, values, 5);


    test_queue.size = 2;
    test_queue.head = headValue;
    test_queue.tail = tailValue;

    writer_routine(&test_writer);

    unsigned char* data_buffer = buffer;

    for (int i = 0; i < 5; i++){
        ASSERT_EQUAL(data_buffer[i], i + 1);
    }

    return PASS;
}



int test_create_destroy_mem_writer(){
    struct sMemWriter* mw = new_mem_writer();

    char* filename = "test.file";

    init_mem_writer(mw, filename);


    ASSERT_STR_EQUAL(mw->filename, filename);

    ASSERT_NOT_NULL((void*) mw->file);

    ASSERT_EQUAL(mw->hasWrittenHeader, 0)
    ASSERT_NULL(mw->prevTimestamp)

    ASSERT_NOT_NULL(mw->writer_queue)

    ASSERT_NULL(mw->writer_queue->head)
    ASSERT_NULL(mw->writer_queue->tail)
    ASSERT_EQUAL(mw->writer_queue->size, 0)


    // Todo how can we test that we have freed?
    destroy_mem_writer(mw);

//    remove(filename);
    return PASS;
}

int test_write_mtc_header(){
    setenv("TZ", "UTC", 1);
    tzset();

    // 2021-07-19: 16:39.47
    struct timeval* time = malloc(sizeof(struct timeval));
    if(time == NULL){
        perror("Failed to allocated time memory");
        return FAIL;
    }
    time->tv_sec = 1629387587;
    time->tv_usec = 510000;

    ushort* buffer = write_mtc_header(time);

    ASSERT_EQUAL(buffer[0],  1);

    ASSERT_EQUAL(buffer[1], 85);
    ASSERT_EQUAL(buffer[2], 486);
    ASSERT_EQUAL(buffer[3], 249);
    ASSERT_EQUAL(buffer[4], 2543);


    free(buffer);
    free(time);

    unsetenv("TZ");
    tzset();
    return PASS;
}




int test_write_mem_header() {
    struct mem_queue test_queue;

    mem_queue_init(&test_queue);

    struct sMemWriter test_writer = {"test", NULL, 0, 0, NULL, &test_queue, 0};

    struct sMemInfo meminfo = {0};
    struct sMemVmInfo memvminfo = {0};

    write_mem(&test_writer, &meminfo, &memvminfo, NULL);

    ASSERT_EQUAL(test_writer.hasWrittenHeader, 1)

    ASSERT_EQUAL(test_queue.size, 1);
    ASSERT_NOT_NULL(test_queue.head);
    ASSERT_NOT_NULL(test_queue.tail);

    struct mem_value* header_value = test_queue.head;

    ASSERT_EQUAL(header_value->value->length, 5);
    ushort* header_content = header_value->value->data;

    ASSERT_EQUAL(header_content[0], 1);
    // Harder to test timestamps since we get current time. This is tested above.

    return PASS;
}

void set_struct_incremental_values(unsigned long* sStruct, const uint structLength, const uint offset) {
    for (int i =0; i < structLength / SIZE_UL; i++) {
        sStruct[i] = i + 1 + offset;
    }
}


int test_write_mem_body_1() {
    struct mem_queue test_queue;

    mem_queue_init(&test_queue);

    struct sMemWriter test_writer = {"test", NULL, 0, 1, get_current_time(), &test_queue, 0};

    struct sMemInfo meminfo = {0};
    struct sMemVmInfo memvminfo = {0};

    set_struct_incremental_values((unsigned long*) (&memvminfo), sizeof(struct sMemVmInfo), 0);

    set_struct_incremental_values((unsigned long*) (&meminfo), sizeof(struct sMemInfo), sizeof(struct sMemVmInfo) / SIZE_UL);

    write_mem(&test_writer, &meminfo, &memvminfo, NULL);

    struct mem_value* header_value = test_queue.head;

    // 4 is the subheader containing the millisecond offset and length.
    uint total_size = 4 + STRUCT_WRITE_SIZE(struct sMemVmInfo) + STRUCT_WRITE_SIZE(struct sMemInfo);

    ASSERT_EQUAL(header_value->value->length, total_size);

    unsigned char* countBuffer = header_value->value->data + 2;
    ushort count = countBuffer[0] << 8 | countBuffer[1];
    ASSERT_EQUAL(count, total_size - 4);

    unsigned char* buffer = header_value->value->data + 4;

    int v = 0;
    for (int i = 0; i < header_value->value->length - 4; i += 3) {
        ASSERT_EQUAL(buffer[i], v); // Key
        short value = buffer[i + 1] << 8 | buffer[i + 2];
        ASSERT_EQUAL(value, v+1);
        v++;
    }

    return PASS;
}

int test_write_mem_body_2() {
    struct mem_queue test_queue;

    mem_queue_init(&test_queue);

    struct sMemWriter test_writer = {"test", NULL, 0, 1, get_current_time(), &test_queue, 0};

    struct sMemInfo meminfo = {0};
    struct sMemVmInfo memvminfo = {0};
    struct sProcessInfo mem_process_info = {0};

    set_struct_incremental_values((unsigned long*) &meminfo, sizeof(struct sMemInfo), 0);

    set_struct_incremental_values((unsigned long*) &memvminfo, sizeof(struct sMemVmInfo), sizeof(struct sMemInfo) / SIZE_UL);

    write_mem(&test_writer, &meminfo, &memvminfo, NULL);

    struct mem_value* header_value = test_queue.head;

    // 4 is the subheader containing the millisecond offset and length.
    uint total_size = 4 + STRUCT_WRITE_SIZE(struct sMemInfo) + STRUCT_WRITE_SIZE(struct sMemVmInfo);

    ASSERT_EQUAL(header_value->value->length, total_size);

    char* buffer = header_value->value->data + 5;

    int v = 0;
    for (int i = 0; i < header_value->value->length - 5; i += 3) {
        ASSERT_EQUAL(buffer[i], v); // Key
        short value = buffer[i + 1] << 8 | buffer[i + 2];
        ASSERT_EQUAL(value, v+1);
        v++;
    }

    return PASS;

    return PASS;
}

