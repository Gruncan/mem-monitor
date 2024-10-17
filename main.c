#include <stdio.h>
#include <stdlib.h>

#include "mem-info.h"


int main(void){
    struct sMemInfo* mi = malloc(sizeof(struct sMemInfo));

    read_mem_info(mi);

    printf("Memory info:\n");
    printf(" - Total: %lu\n", mi->total);
    printf(" - Free: %lu\n", mi->free);
    printf(" - Available: %lu\n", mi->available);

    free(mi);

    return 0;
}
