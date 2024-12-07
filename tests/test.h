
#ifndef MEM_MONITOR_TEST_H
#define MEM_MONITOR_TEST_H

#define RED_TEXT   "\033[0;31m"
#define GREEN_TEXT "\033[0;32m"
#define RESET_TEXT "\033[0m"

#define INIT_TEST static int count = 0; \
                  static int passed = 0;\
                  printf("Running tests..\n"); \


#define PRINT_RESULTS \
if (count == passed) { \
    printf(GREEN_TEXT "Passed %d/%d!\n" RESET_TEXT, passed, count);\
} else {              \
    printf(RED_TEXT "Passed %d/%d!\n" RESET_TEXT, passed, count); \
} \

#define TEST(func) \
    if (func()) {  \
        count++;   \
        passed++;\
        printf(GREEN_TEXT "  - Passed %s\n" RESET_TEXT, #func); \
    } else {       \
        count++;           \
        printf(RED_TEXT "  - Failed %s\n" RESET_TEXT, #func);   \
    } \


#define ASSERT_EQUAL(value, expected) \
    if (value != expected) {          \
        printf("%s != %s\n", #value, #expected); \
        return 0;                               \
    }

#define EXIT_TESTS return count == passed ? 0 : -1;


#endif //MEM_MONITOR_TEST_H
