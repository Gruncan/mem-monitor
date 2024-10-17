#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem-info.h"
#include "mem-writer.h"


#define SLEEP_TIME 5000

int main(void){
    struct sMemInfo* mi = malloc(sizeof(struct sMemInfo));
    struct sMemPageInfo* mp = malloc(sizeof(struct sMemPageInfo));


    read_mem_info(mi);
    read_mem_pages(mp);


    printf("Memory info:\n");
    printf(" - Total: %lu\n", mi->total);
    printf(" - Free: %lu\n", mi->free);
    printf(" - Available: %lu\n", mi->available);

    printf("\nPage info:\n");
    printf(" - pgfault: %lu\n", mp->pgfault);
    printf(" - pgmajfault: %lu\n", mp->pgmajfault);
    printf(" - pgpgin: %lu\n", mp->pgpgin);
    printf(" - pgpgout: %lu\n", mp->pgpgout);

    struct sMemWriter* mw = malloc(sizeof(struct sMemWriter));

    init_mem_writer(mw, "memlog.json");


    while (1) {
        read_mem_info(mi);
        write_mem(mw, mi);
        usleep(SLEEP_TIME);
    }



    free(mi);
    free(mp);

    return 0;
}
