
#include <assert.h>

#include "mem-writer.h"

#include "test.h"

int test1();

int main(int argc, char *argv[]){
    INIT_TEST

    TEST(test1)

    PRINT_RESULTS

    EXIT_TESTS
}


struct testStruct {
    unsigned long v1;
    unsigned long v2;
    unsigned long v3;
    unsigned long v4;
};

int test1() {
    struct testStruct t1 = {1, 2, 3, 4};

    size_t length = sizeof(struct testStruct) / sizeof(unsigned long);
    size_t size = sizeof(struct testStruct);

    char* buffer = malloc(length + 4);

    for (int i = 0; i < length; i++) {
        buffer[i] = 0;
    }

    int offset = write_struct_data(buffer, &t1, size, 0);
    ASSERT_EQUAL(offset, (length * 2) + 5);

    int v = 0;
    for (int i = 0; i < length + 4; i += 3) {
        ASSERT_EQUAL(buffer[i], v); // Key
        short value = buffer[i + 1] << 8 | buffer[i + 2];
        ASSERT_EQUAL(value, v+1);
        v++;
    }

    return 1;
}
