
#ifndef MEM_MONITOR_TEST_H
#define MEM_MONITOR_TEST_H

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
} \


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
    } \




#define ASSERT_EQUAL(value, expected) \
    if (value != expected) {          \
        fprintf(stderr, RED_TEXT "Failed on: %s != %s\n" RESET_TEXT, #value, #expected); \
        fprintf(stderr, RED_TEXT "         : %d != %d\n" RESET_TEXT, value, expected); \
        return FAIL;                             \
    }

#define EXIT_TESTS return count == passed + skipped ? 0 : -1;


#endif //MEM_MONITOR_TEST_H
