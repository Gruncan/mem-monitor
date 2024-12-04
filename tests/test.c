
#include <assert.h>

#include "mem-writer.h"


int test1();

int main(int argc, char *argv[]){
    test1();
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
    assert(offset == (length * 2) + 4);

    int v = 0;
    for (int i = 0; i < length + 4; i += 3) {
        assert(buffer[i] == v); // Key
        short value = buffer[i + 1] << 8 | buffer[i + 2];
        printf("%d: %lu\n", v, value);
        assert(value == v+1);
        v++;
    }
}
