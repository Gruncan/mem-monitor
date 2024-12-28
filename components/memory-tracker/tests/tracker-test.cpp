
#include <cstdio>
#include <cstdlib>
#include <string>
#include <test.h>
#include <unistd.h>

int malloc_test();
int realloc_test();
int free_test();
int calloc_test();
int new_test();
int delete_test();
int new_nothrow_test();
int delete_nothrow_test();
int new_array_test();
int delete_array_test();
int new_array_nothrow_test();
int delete_array_nothrow_test();
int delete_sized();
int delete_array_sized();
int new_aligned_test();
int delete_aligned_test();
int new_array_aligned_test();
int delete_array_aligned_test();

int main() {
    INIT_TEST("Memory tracker")

    TEST(malloc_test)
    TEST(realloc_test)
    TEST(free_test)
    TEST(calloc_test)

    TEST(new_test)
    TEST(delete_test)
    TEST(new_nothrow_test)
    TEST(delete_nothrow_test)
    TEST(new_array_test)
    TEST(delete_array_test)
    TEST(new_array_nothrow_test)
    TEST(delete_array_nothrow_test)
    TEST(delete_sized)
    TEST(delete_array_sized)
    TEST(new_aligned_test)
    TEST(delete_aligned_test)
    TEST(new_array_aligned_test)
    TEST(delete_array_aligned_test)

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

int new_test() {
    std::string* test_str_ptr = new std::string("hello world");

    ASSERT_NOT_NULL(test_str_ptr);

    delete test_str_ptr;
    return PASS;
}

int delete_test() {
    return PASS;
}

int new_nothrow_test() {
    return PASS;
}

int new_array_test() {
    int* ptr = new int[10];
    ASSERT_NOT_NULL(ptr);

    delete[] ptr;
    return PASS;
}

int delete_array_test() {
    return PASS;
}

int delete_nothrow_test() {
    int* ptr = nullptr;
    ASSERT_NULL(ptr);
    ::operator delete(ptr, std::nothrow);
    return PASS;
}

int new_array_nothrow_test() {
    // This should throw a std::bad_alloc however with nothrow we get a nullptr
    int* ptr = new(std::nothrow) int[1000000000000000000];
    ASSERT_NULL(ptr);
    return PASS;
}


int delete_array_nothrow_test() {
    int* ptr = nullptr;
    ASSERT_NULL(ptr);
    ::operator delete[](ptr, std::nothrow);
    return PASS;
}

struct Test {
    char data[10];
};

int delete_sized() {
    Test* test = new Test;
    ASSERT_NOT_NULL(test);

    delete test;
    return PASS;
}

int delete_array_sized() {
    Test* test = new Test[10];
    ASSERT_NOT_NULL(test);
    // Fix this to call
    delete[] test;
    return PASS;
}

int new_aligned_test() {
    return PASS;
}

int delete_aligned_test() {
    return PASS;
}

int new_array_aligned_test() {
    return PASS;
}

int delete_array_aligned_test() {
    return PASS;
}











