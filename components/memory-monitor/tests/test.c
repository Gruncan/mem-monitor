
#include "mem-reader.h"
#include "mem-threading.h"
#include "mem-writer.h"
#include "process-reader.h"

#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#define STRUCT_WRITE_SIZE(s) ((sizeof(s) / SIZE_UL) * MTC_VALUE_WRITE_OFFSET)


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
int test_decoder();

static const uint SIZE_UL = sizeof(unsigned long);


int main(int argc, char* argv[]) {
    INIT_TEST("Memory Monitor")

    TEST(test_struct_writer)

    TEST(test_data_writer1)
    TEST(test_data_writer2)
    TEST(test_data_writer3)

    TEST(test_timeval_diff_ms)

    TEST(test_writer_routine)

    TEST(test_create_destroy_mem_writer)

    TEST_SKIP(test_write_mtc_header)
    TEST_SKIP(test_write_mem_header)

    TEST(test_write_mem_body_1)

    TEST(test_write_mem_body_2)


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
    ASSERT_EQUAL(offset, (length * (MTC_VALUE_WRITE_OFFSET - 1)) + 4);

    int v = 0;
    for (int i = 0; i < length + 4; i += 4) {
        ASSERT_EQUAL(buffer[i], v); // Key
        ushort value = ((ushort) buffer[i + 1] << 16) | ((ushort) buffer[i + 2] << 8) | buffer[i + 3];
        ASSERT_EQUAL(value, v + 1);
        v++;
    }

    free(buffer);

    return PASS;
}

int test_data_writer1() {
    char* buffer = malloc(MTC_VALUE_WRITE_OFFSET);

    memset(buffer, 0, MTC_VALUE_WRITE_OFFSET);

    write_data_content(buffer, 0, 1, MTC_VALUE_WRITE_OFFSET);

    ASSERT_EQUAL(buffer[0], 1);
    ASSERT_EQUAL(buffer[1], 0);
    ASSERT_EQUAL(buffer[2], 0);
    ASSERT_EQUAL(buffer[3], 4);

    return PASS;
}

int test_data_writer2() {
    char* buffer = malloc(3 + MTC_VALUE_WRITE_OFFSET);

    memset(buffer, 0, 3 + MTC_VALUE_WRITE_OFFSET);


    write_data_content(buffer, 3, 1, MTC_VALUE_WRITE_OFFSET);

    ASSERT_EQUAL(buffer[3], 1);
    ASSERT_EQUAL(buffer[4], 0);
    ASSERT_EQUAL(buffer[5], 0);
    ASSERT_EQUAL(buffer[6], 4);

    return PASS;
}

int test_data_writer3() {
    char* buffer = malloc(MTC_VALUE_WRITE_OFFSET);

    memset(buffer, 0, MTC_VALUE_WRITE_OFFSET);

    write_data_content(buffer, 0, 1, 65000);

    ASSERT_EQUAL(buffer[0], 1);
    ASSERT_EQUAL(buffer[1], 0);
    ASSERT_EQUAL((unsigned char) buffer[2], 253);
    ASSERT_EQUAL((unsigned char) buffer[3], 232)

    return PASS;
}

int test_timeval_diff_ms() {
    struct timeval time_1 = {1629387587, 500000};
    struct timeval time_2 = {1629387587, 510000};

    ushort milliseconds1 = timeval_diff_ms(&time_1, &time_2);

    ASSERT_EQUAL(milliseconds1, 10);

    ushort milliseconds2 = timeval_diff_ms(&time_2, &time_1);

    ASSERT_EQUAL(milliseconds2, 0)

    return PASS;
}

struct mem_value_s* init_test_mem_value(struct mem_value_s* next, void* data, unsigned int length) {
    struct mem_value_s* test_value = malloc(sizeof(struct mem_value_s));
    test_value->next = next;

    struct mtc_value_s* test_mtc_value = malloc(sizeof(struct mtc_value_s));
    test_mtc_value->data = data;
    test_mtc_value->length = length;

    test_value->value = test_mtc_value;

    return test_value;
}


int test_writer_routine() {
    MemQueue test_queue;

    mem_queue_init(&test_queue);

    struct mem_writer_s test_writer = {"test", NULL, 0, 0, NULL, &test_queue, 0};

    void* buffer = malloc(5);

    test_writer.file = buffer;

    struct mem_value_s* tail_value = init_test_mem_value(NULL, NULL, 0);

    unsigned char* values = malloc(5);

    for (int i = 0; i < 5; i++) {
        values[i] = i + 1;
    }

    struct mem_value_s* head_value = init_test_mem_value(tail_value, values, 5);


    test_queue.size = 2;
    test_queue.head = head_value;
    test_queue.tail = tail_value;

    writer_routine(&test_writer);

    unsigned char* data_buffer = buffer;

    for (int i = 0; i < 5; i++) {
        ASSERT_EQUAL(data_buffer[i], i + 1);
    }

    return PASS;
}


int test_create_destroy_mem_writer() {
    struct mem_writer_s* mw = new_mem_writer();

    char* filename = "test.file";

    init_mem_writer(mw, filename);


    ASSERT_STR_EQUAL(mw->filename, filename);

    ASSERT_NOT_NULL((void*) mw->file);

    ASSERT_EQUAL(mw->has_written_header, 0)
    ASSERT_NULL(mw->prev_timestamp)

    ASSERT_NOT_NULL(mw->writer_queue)

    ASSERT_NULL(mw->writer_queue->head)
    ASSERT_NULL(mw->writer_queue->tail)
    ASSERT_EQUAL(mw->writer_queue->size, 0)


    // Todo how can we test that we have freed?
    destroy_mem_writer(mw);

    //    remove(filename);
    return PASS;
}

int test_write_mtc_header() {
    setenv("TZ", "UTC", 1);
    tzset();

    // 2021-07-19: 16:39.47
    struct timeval* time = malloc(sizeof(struct timeval));
    if (time == NULL) {
        perror("Failed to allocated time memory");
        return FAIL;
    }
    time->tv_sec = 1629387587;
    time->tv_usec = 510000;

    unsigned char* buffer = write_mtc_header(time, 1);

    ASSERT_EQUAL(buffer[0], 1);

    ASSERT_EQUAL(buffer[1], 85);
    ASSERT_EQUAL(buffer[2], 231);
    ASSERT_EQUAL(buffer[3], 9);
    ASSERT_EQUAL(buffer[4], 269);


    free(buffer);
    free(time);

    unsetenv("TZ");
    tzset();
    return PASS;
}


int test_write_mem_header() {
    MemQueue test_queue;

    mem_queue_init(&test_queue);

    struct mem_writer_s test_writer = {"test", NULL, 0, 0, NULL, &test_queue, 0};

    MemInfo meminfo = {0};
    MemVmInfo memvminfo = {0};

    write_mem(&test_writer, &meminfo, &memvminfo, NULL);

    ASSERT_EQUAL(test_writer.has_written_header, 1)

    ASSERT_EQUAL(test_queue.size, 1);
    ASSERT_NOT_NULL(test_queue.head);
    ASSERT_NOT_NULL(test_queue.tail);

    struct mem_value_s* header_value = test_queue.head;

    ASSERT_EQUAL(header_value->value->length, 5);
    ushort* header_content = header_value->value->data;

    ASSERT_EQUAL(header_content[0], 1);
    // Harder to test timestamps since we get current time. This is tested above.

    return PASS;
}

void set_struct_incremental_values(unsigned long* struct_ptr, const uint struct_length, const uint offset) {
    for (int i = 0; i < struct_length / SIZE_UL; i++) {
        struct_ptr[i] = i + 1 + offset;
    }
}


int test_write_mem_body_1() {
    MemQueue test_queue;

    mem_queue_init(&test_queue);

    MemWriter test_writer = {"test", NULL, 0, 1, get_current_time(), &test_queue, 0};

    MemInfo mem_info = {0};
    MemVmInfo mem_vm_info = {0};

    set_struct_incremental_values((unsigned long*) (&mem_vm_info), sizeof(MemVmInfo), 0);

    set_struct_incremental_values((unsigned long*) (&mem_info), sizeof(MemInfo), sizeof(MemVmInfo) / SIZE_UL);

    write_mem(&test_writer, &mem_info, &mem_vm_info, NULL);

    struct mem_value_s* header_value = test_queue.head;

    // 4 is the subheader containing the millisecond offset and length.
    uint total_size = 4 + STRUCT_WRITE_SIZE(MemVmInfo) + STRUCT_WRITE_SIZE(MemInfo);

    ASSERT_EQUAL(header_value->value->length, total_size);

    unsigned char* countBuffer = header_value->value->data + 2;
    ushort count = countBuffer[0] << 8 | countBuffer[1];
    ASSERT_EQUAL(count, total_size - 4);

    unsigned char* buffer = header_value->value->data + 4;


    int v = 0;
    for (int i = 0; i < header_value->value->length - 4; i += MTC_VALUE_WRITE_OFFSET) {
        ASSERT_EQUAL(buffer[i], v); // Key
        short value = ((ushort) buffer[i + 1] << 16) | ((ushort)buffer[i+2] << 8) | (ushort) buffer[i + 3];
        ASSERT_EQUAL(value, v + 1);
        v++;
    }

    return PASS;
}

int test_write_mem_body_2() {
    MemQueue test_queue;

    mem_queue_init(&test_queue);

    struct mem_writer_s test_writer = {"test", NULL, 0, 1, get_current_time(), &test_queue, 0};

    MemInfo mem_info = {0};
    MemVmInfo mem_vm_info = {0};
    MemProcInfo mem_proc_info = {0};

    set_struct_incremental_values((unsigned long*) &mem_vm_info, sizeof(MemVmInfo), 0);

    size_t value_length = sizeof(MemVmInfo) / SIZE_UL;
    set_struct_incremental_values((unsigned long*) &mem_info, sizeof(MemInfo), value_length);

    value_length += sizeof(MemInfo) / SIZE_UL;
    set_struct_incremental_values((unsigned long*) &mem_proc_info, sizeof(MemProcInfo), value_length);

    write_mem(&test_writer, &mem_info, &mem_vm_info, NULL);

    struct mem_value_s* header_value = test_queue.head;

    // 4 is the subheader containing the millisecond offset and length.
    uint total_size = 4 + STRUCT_WRITE_SIZE(MemVmInfo) + STRUCT_WRITE_SIZE(MemInfo);

    ASSERT_EQUAL(header_value->value->length, total_size);

    unsigned char* count_buffer = header_value->value->data + 2;
    ushort count = count_buffer[0] << 8 | count_buffer[1];
    ASSERT_EQUAL(count, total_size - 4);

    unsigned char* buffer = header_value->value->data + 4;

    int v = 0;
    for (int i = 0; i < header_value->value->length - 4; i += MTC_VALUE_WRITE_OFFSET) {
        ASSERT_EQUAL(buffer[i], v); // Key
        ushort value = ((ushort) buffer[i + 1] << 16) | (buffer[i + 2] << 8) | buffer[i + 3];
        ASSERT_EQUAL(value, v + 1);
        v++;
    }

    return PASS;
}

int test_decoder() {
    MemQueue test_queue;
    mem_queue_init(&test_queue);

    MemWriter test_writer;

#undef MEM_TEST // TODO fix this
    init_mem_writer(&test_writer, "test_writing.mtc");


    MemInfo mem_info = {0};
    MemVmInfo mem_vm_info = {0};
    MemProcInfo mem_proc_info = {0};

    set_struct_incremental_values((unsigned long*) &mem_vm_info, sizeof(MemVmInfo), 0);

    size_t value_length = sizeof(MemVmInfo) / SIZE_UL;
    set_struct_incremental_values((unsigned long*) &mem_info, sizeof(MemInfo), value_length);

    value_length += sizeof(MemInfo) / SIZE_UL;
    set_struct_incremental_values((unsigned long*) &mem_proc_info, sizeof(MemProcInfo), value_length);

    write_mem(&test_writer, &mem_info, &mem_vm_info, &mem_proc_info);

    write_mem(&test_writer, &mem_info, &mem_vm_info, &mem_proc_info);

    write_mem(&test_writer, &mem_info, &mem_vm_info, &mem_proc_info);


    return PASS;
}
