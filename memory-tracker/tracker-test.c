

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Starting test...\n");
    void* t = malloc(10);
    free(t);
    printf("Done.\n");
    sleep(30);
    return 0;
}