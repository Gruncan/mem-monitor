
#include "mem-writer.h"
#include "mem-threading.h"

#include "test.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int test_struct_writer();
int test_data_writer1();
int test_data_writer2();
int test_data_writer3();
int test_timeval_diff_ms();
int test_writer_routine();
int test_create_destroy_mem_writer();
int test_write_mtc_header();

int main(int argc, char *argv[]){
    INIT_TEST

    TEST(test_struct_writer)

    TEST(test_data_writer1)
    TEST(test_data_writer2)
    TEST(test_data_writer3)

    TEST(test_timeval_diff_ms)

    TEST(test_writer_routine)

    TEST(test_create_destroy_mem_writer)

    TEST(test_write_mtc_header) // Not sure why this fails only on remote ;(

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


    int offset = write_struct_data(buffer, &t1, size, 0);
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

