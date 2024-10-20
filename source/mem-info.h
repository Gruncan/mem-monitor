#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stddef.h>

struct sMemAllInfo {

};

struct __attribute__((packed)) sMemPageInfo {
    unsigned long pgfault;
    unsigned long pgmajfault;
    unsigned long pgpgin;
    unsigned long pgpgout;
};

struct sMemProcPageInfo {

};

struct __attribute__((packed)) sMemInfo {
    unsigned long total;
    unsigned long free;
    unsigned long available;
    unsigned long buffers;
    unsigned long cached;
    unsigned long swapCache;

    unsigned long active;
    unsigned long inActive;
    unsigned long activeAnon;
    unsigned long inActiveAnon;
    unsigned long activeFile;
    unsigned long inActiveFile;

    unsigned long unevictable;
    unsigned long mLocked;

    unsigned long swapTotal;
    unsigned long swapFree;
    unsigned long zswap;
    unsigned long zswapped;

    unsigned long dirty;
    unsigned long writeback;

    unsigned long pagesAnon;
    unsigned long pageMapped;

    unsigned long shmem;

    unsigned long kreClaimable;
    unsigned long slab;
    unsigned long srClaimable;
    unsigned long sunReclaim;

    unsigned long kernelStack;
    unsigned long pageTables;
    unsigned long secPageTables;
    unsigned long nfsUnstable;
    unsigned long bounce;
    unsigned long writebackTmp;

    unsigned long commitLimit;
    unsigned long committedAllocs;

    unsigned long vmallocTotal;
    unsigned long vmallocUsed;
    unsigned long vmallocChunk;

    unsigned long perCPU;
    unsigned long hardwareCorrupted;

    unsigned long hugePagesAnon;
    unsigned long hugePagesShmem;
    unsigned long pmdMappedShmem;
    unsigned long hugePagesFile;
    unsigned long pmdMappedFile;
    unsigned long unaccepted;

    unsigned long hugePagesTotal;
    unsigned long hugePagesFree;
    unsigned long hugePagesRsvd;
    unsigned long hugePagesSurp;
    unsigned long hugePageSize;
    unsigned long hugePageTLB;

    unsigned long directMap4k;
    unsigned long directMap2M;
    unsigned long directMap1G;

};


struct memInfoStrings {
    char** mem_strings;
    size_t mem_strings_count;
};


void read_mem_info(struct sMemInfo* mi);

void read_mem_pages(struct sMemPageInfo* mp);

char* get_mem_page_data(struct sMemPageInfo* mp, const char* name);

struct memInfoStrings* get_all_mem_page_data(struct sMemPageInfo* mp);

char* get_mem_info_data(struct sMemInfo* mi, const char* name);

struct memInfoStrings* get_all_mem_info_data(struct sMemInfo* mi);

void destroy_all_mem_data(struct memInfoStrings* ms);


const char** get_mem_info_names();

const char** get_mem_page_names();

#endif //MEM_INFO_H
