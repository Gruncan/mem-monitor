

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Starting test\n");
    int* arr = new int[10];
    delete[] arr;


    void* test = malloc(5);
    printf("Allocated memory: %p\n", test);
    free(test);
    printf("Finished test\n");
    // std::string* arr2 = new std::string[20];
    //
    // arr2[0] = "test";
    //
    // // printf("Starting test...\n");
    // // void* t = malloc(10);
    // // free(t);
    // // printf("Done.\n");
    // // sleep(30);
    // delete[] arr2;
    sleep(10);
    return 0;
}
