
#ifndef MEM_MONITOR_TEST_H
#define MEM_MONITOR_TEST_H

#include <string.h>


#define RED_TEXT   "\033[0;31m"
#define GREEN_TEXT "\033[0;32m"
#define YELLOW_TEXT "\033[0;33m"
#define RESET_TEXT "\033[0m"

#define PASS 1
#define FAIL 0

#define INIT_TEST static int count = 0; \
                  static int passed = 0; \
                  static int skipped = 0; \
                  printf("Running tests..\n"); \


#define PRINT_RESULTS \
if(skipped != 0){     \
    printf(YELLOW_TEXT "Skipped %d!\n" RESET_TEXT, skipped);    \
}                      \
if (count == passed + skipped) { \
    printf(GREEN_TEXT "Passed %d/%d!\n" RESET_TEXT, passed, count);\
} else {              \
    printf(RED_TEXT "Passed %d/%d!\n" RESET_TEXT, passed, count); \
}                     \
fflush(stdout); \


#define TEST_SKIP(func) skipped++; \
                        count++;   \
                        printf(YELLOW_TEXT "  - Skipped %s\n" RESET_TEXT, #func);

#define TEST(func) \
    if (func()) {  \
        count++;   \
        passed++;\
        printf(GREEN_TEXT "  - Passed %s\n" RESET_TEXT, #func); \
    } else {       \
        count++;           \
        printf(RED_TEXT "  - Failed %s\n" RESET_TEXT, #func);   \
    }              \
    fflush(stdout); \


#define ASSERT_EQUAL_P(value, expected) \
    if (value != expected) {          \
        fprintf(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected); \
        fprintf(stderr, RED_TEXT "         : %p != %p\n" RESET_TEXT, value, expected); \
        return FAIL;                             \
    }


#define ASSERT_EQUAL(value, expected) \
    if (value != expected) {          \
        fprintf(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected); \
        fprintf(stderr, RED_TEXT "         : %d != %d\n" RESET_TEXT, value, expected); \
        return FAIL;                             \
    }

#define ASSERT_STR_EQUAL(value, expected) \
    if (strcmp(value, expected) != 0) {          \
        fprintf(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected); \
        fprintf(stderr, RED_TEXT "         : %s != %s\n" RESET_TEXT, value, expected); \
        return FAIL;                             \
    }



#define ASSERT_NOT_EQUAL_P(value, expected) \
    if (value == expected) {              \
        fprintf(stderr, RED_TEXT "Failed on: %s == %s\n" RESET_TEXT, #value, #expected); \
        fprintf(stderr, RED_TEXT "         : %p == %p\n" RESET_TEXT, value, expected); \
        return FAIL;                                          \
    }

#define ASSERT_NOT_EQUAL(value, expected) \
    if (value == expected) {              \
        fprintf(stderr, RED_TEXT "Failed on: %s == %s\n" RESET_TEXT, #value, #expected); \
        fprintf(stderr, RED_TEXT "         : %d == %d\n" RESET_TEXT, value, expected); \
        return FAIL;                                          \
    }                                     \


#define ASSERT_STR_NOT_EQUAL(value, expected) \
    ASSERT_EQUAL(strcmp(value, expected), 0)


#define ASSERT_NOT_NULL(value) \
    ASSERT_NOT_EQUAL_P(value, NULL)

#define ASSERT_NULL(value) \
    ASSERT_EQUAL_P(value, NULL)

#define EXIT_VALUE (count == (passed + skipped) ? 0 : -1)


#endif //MEM_MONITOR_TEST_H
