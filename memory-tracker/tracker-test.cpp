
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "../tests/test.h"


int malloc_test();
int realloc_test();
int free_test();
int calloc_test();


int main() {
    INIT_TEST("Memory tracker")


    TEST(malloc_test)
    TEST(realloc_test)
    TEST(free_test)
    TEST(calloc_test)

    PRINT_RESULTS

    return EXIT_VALUE;
}


int malloc_test() {
    void* malloc_ptr = malloc(10);

    ASSERT_NOT_NULL(malloc_ptr);

    void* realloc_ptr = realloc(malloc_ptr, 20);
    ASSERT_NOT_NULL(realloc_ptr);

    free(realloc_ptr);

    return PASS;
}

// Since we require malloc no need to run separate malloc call
int realloc_test() {
    return PASS;
}

int free_test() {
    return PASS;
}

int calloc_test() {
    void* calloc_ptr = calloc(5, sizeof(int));
    ASSERT_NOT_NULL(calloc_ptr);

    return PASS;
}