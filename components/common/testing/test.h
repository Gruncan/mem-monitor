
#ifndef MEM_MONITOR_TEST_H
#define MEM_MONITOR_TEST_H

#include <string.h>

#ifdef __cplusplus
#define NONE nullptr
#else
#define NONE NULL
#endif

#ifdef TEST_NO_PRINT
#define TEST_PRINT(...)
#define TEST_PRINT_ERR(...)
#else
#define TEST_PRINT(str, ...) printf(str, ##__VA_ARGS__);
#define TEST_PRINT_ERR(str, ...) fprintf(stderr, str, ##__VA_ARGS__);
#endif

#define RED_TEXT "\033[0;31m"
#define GREEN_TEXT "\033[0;32m"
#define YELLOW_TEXT "\033[0;33m"
#define RESET_TEXT "\033[0m"

#define PASS 1
#define FAIL 0

#define INIT_TEST(name)                                                                                                \
    static int count = 0;                                                                                              \
    static int passed = 0;                                                                                             \
    static int skipped = 0;                                                                                            \
    TEST_PRINT("[%s] Running tests..\n", name);


#define PRINT_RESULTS                                                                                                  \
    if (skipped != 0) {                                                                                                \
        TEST_PRINT(YELLOW_TEXT "Skipped %d!\n" RESET_TEXT, skipped);                                                       \
    }                                                                                                                  \
    if (count == passed + skipped) {                                                                                   \
        TEST_PRINT(GREEN_TEXT "Passed %d/%d!\n" RESET_TEXT, passed, count);                                                \
    } else {                                                                                                           \
        TEST_PRINT(RED_TEXT "Passed %d/%d!\n" RESET_TEXT, passed, count);                                                  \
    }                                                                                                                  \
    fflush(stdout);


#define TEST_SKIP(func)                                                                                                \
    skipped++;                                                                                                         \
    count++;                                                                                                           \
    TEST_PRINT(YELLOW_TEXT "  - Skipped %s\n" RESET_TEXT, #func);

#define TEST(func)                                                                                                     \
    if (func()) {                                                                                                      \
        count++;                                                                                                       \
        passed++;                                                                                                      \
        TEST_PRINT(GREEN_TEXT "  - Passed %s\n" RESET_TEXT, #func);                                                        \
    } else {                                                                                                           \
        count++;                                                                                                       \
        TEST_PRINT(RED_TEXT "  - Failed %s\n" RESET_TEXT, #func);                                                          \
    }                                                                                                                  \
    fflush(stdout);


#define ASSERT_EQUAL_P(value, expected)                                                                                \
    if (value != expected) {                                                                                           \
        TEST_PRINT_ERR(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected);                               \
        TEST_PRINT_ERR(stderr, RED_TEXT "         : %p != %p\n" RESET_TEXT, value, expected);                                 \
        return FAIL;                                                                                                   \
    }


#define ASSERT_EQUAL(value, expected)                                                                                  \
    if (value != expected) {                                                                                           \
        TEST_PRINT_ERR(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected);                               \
        TEST_PRINT_ERR(stderr, RED_TEXT "         : %d != %d\n" RESET_TEXT, value, expected);                                 \
        return FAIL;                                                                                                   \
    }

#define ASSERT_STR_EQUAL(value, expected)                                                                              \
    if (strcmp(value, expected) != 0) {                                                                                \
        TEST_PRINT_ERR(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected);                               \
        TEST_PRINT_ERR(stderr, RED_TEXT "         : %s != %s\n" RESET_TEXT, value, expected);                                 \
        return FAIL;                                                                                                   \
    }


#define ASSERT_NOT_EQUAL_P(value, expected)                                                                            \
    if (value == expected) {                                                                                           \
        TEST_PRINT_ERR(stderr, RED_TEXT "Failed on: %s == %s\n" RESET_TEXT, #value, #expected);                               \
        TEST_PRINT_ERR(stderr, RED_TEXT "         : %p == %p\n" RESET_TEXT, value, expected);                                 \
        return FAIL;                                                                                                   \
    }

#define ASSERT_NOT_EQUAL(value, expected)                                                                              \
    if (value == expected) {                                                                                           \
        TEST_PRINT_ERR(stderr, RED_TEXT "Failed on: %s == %s\n" RESET_TEXT, #value, #expected);                               \
        TEST_PRINT_ERR(stderr, RED_TEXT "         : %d == %d\n" RESET_TEXT, value, expected);                                 \
        return FAIL;                                                                                                   \
    }


#define ASSERT_STR_NOT_EQUAL(value, expected) ASSERT_EQUAL(strcmp(value, expected), 0)


#define ASSERT_NOT_NULL(value) ASSERT_NOT_EQUAL_P(value, NONE)

#define ASSERT_NULL(value) ASSERT_EQUAL_P(value, NONE)

#define EXIT_VALUE (count == (passed + skipped) ? 0 : -1)


#endif // MEM_MONITOR_TEST_H
