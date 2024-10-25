#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stddef.h>

struct sMemAllInfo {

};

struct __attribute__((packed)) sMemVmInfo {
    // Zone-related stats
    unsigned long nrFreePages;
    unsigned long nrZoneInactiveAnon;
    unsigned long nrZoneActiveAnon;
    unsigned long nrZoneInactiveFile;
    unsigned long nrZoneActiveFile;
    unsigned long nrZoneUnevictable;
    unsigned long nrZoneWritePending;
    unsigned long nrMlock;
    unsigned long nrBounce;
    unsigned long nrZspages;
    unsigned long nrFreeCma;
    unsigned long nrUnaccepted;

    // NUMA-related stats
    unsigned long numaHit;
    unsigned long numaMiss;
    unsigned long numaForeign;
    unsigned long numaInterleave;
    unsigned long numaLocal;
    unsigned long numaOther;

    // Page usage
    unsigned long nrInactiveAnon;
    unsigned long nrActiveAnon;
    unsigned long nrInactiveFile;
    unsigned long nrActiveFile;
    unsigned long nrUnevictable;
    unsigned long nrSlabReclaimable;
    unsigned long nrSlabUnreclaimable;
    unsigned long nrIsolatedAnon;
    unsigned long nrIsolatedFile;

    // Working set
    unsigned long workingsetNodes;
    unsigned long workingsetRefaultAnon;
    unsigned long workingsetRefaultFile;
    unsigned long workingsetActivateAnon;
    unsigned long workingsetActivateFile;
    unsigned long workingsetRestoreAnon;
    unsigned long workingsetRestoreFile;
    unsigned long workingsetNodereclaim;

    // Anonymous and file pages
    unsigned long nrAnonPages;
    unsigned long nrMapped;
    unsigned long nrFilePages;
    unsigned long nrDirty;
    unsigned long nrWriteback;
    unsigned long nrWritebackTemp;
    unsigned long nrShmem;
    unsigned long nrShmemHugepages;
    unsigned long nrShmemPmdmapped;
    unsigned long nrFileHugepages;
    unsigned long nrFilePmdmapped;
    unsigned long nrAnonTransparentHugepages;

    // Dirty and writeback thresholds
    unsigned long nrDirtyThreshold;
    unsigned long nrDirtyBackgroundThreshold;

    // Page reclaim and fault
    unsigned long pgpgin;
    unsigned long pgpgout;
    unsigned long pswpin;
    unsigned long pswpout;
    unsigned long pgfault;
    unsigned long pgmajfault;

    // Page allocation
    unsigned long pgallocDma;
    unsigned long pgallocDma32;
    unsigned long pgallocNormal;
    unsigned long pgallocMovable;
    unsigned long pgallocDevice;
    unsigned long allocstallDma;
    unsigned long allocstallDma32;
    unsigned long allocstallNormal;
    unsigned long allocstallMovable;
    unsigned long allocstallDevice;
    unsigned long pgskipDma;
    unsigned long pgskipDma32;
    unsigned long pgskipNormal;
    unsigned long pgskipMovable;
    unsigned long pgskipDevice;
    unsigned long pgfree;
    unsigned long pgactivate;
    unsigned long pgdeactivate;
    unsigned long pglazyfree;
    unsigned long pglazyfreed;
    unsigned long pgrefill;
    unsigned long pgreuse;

    // Stealing and scanning
    unsigned long pgstealKswapd;
    unsigned long pgstealDirect;
    unsigned long pgstealKhugepaged;
    unsigned long pgscanKswapd;
    unsigned long pgscanDirect;
    unsigned long pgscanKhugepaged;
    unsigned long pgscanDirectThrottle;
    unsigned long pgscanAnon;
    unsigned long pgscanFile;
    unsigned long pgstealAnon;
    unsigned long pgstealFile;

    // Zone reclaim and kswapd
    unsigned long zoneReclaimFailed;
    unsigned long pginodesteal;
    unsigned long slabsScanned;
    unsigned long kswapdInodesteal;
    unsigned long kswapdLowWmarkHitQuickly;
    unsigned long kswapdHighWmarkHitQuickly;
    unsigned long pageoutrun;
    unsigned long pgrotated;
    unsigned long dropPagecache;
    unsigned long dropSlab;
    unsigned long oomKill;

    // NUMA and migration
    unsigned long numaPteUpdates;
    unsigned long numaHugePteUpdates;
    unsigned long numaHintFaults;
    unsigned long numaHintFaultsLocal;
    unsigned long numaPagesMigrated;
    unsigned long pgmigrateSuccess;
    unsigned long pgmigrateFail;
    unsigned long thpMigrationSuccess;
    unsigned long thpMigrationFail;
    unsigned long thpMigrationSplit;

    // Compaction stats
    unsigned long compactMigrateScanned;
    unsigned long compactFreeScanned;
    unsigned long compactIsolated;
    unsigned long compactStall;
    unsigned long compactFail;
    unsigned long compactSuccess;
    unsigned long compactDaemonWake;
    unsigned long compactDaemonMigrateScanned;
    unsigned long compactDaemonFreeScanned;

    // High-order allocations
    unsigned long htlbBuddyAllocSuccess;
    unsigned long htlbBuddyAllocFail;

    // Unevictable pages
    unsigned long unevictablePgsCulled;
    unsigned long unevictablePgsScanned;
    unsigned long unevictablePgsRescued;
    unsigned long unevictablePgsMlocked;
    unsigned long unevictablePgsMunlocked;
    unsigned long unevictablePgsCleared;
    unsigned long unevictablePgsStranded;

    // Transparent hugepage stats
    unsigned long thpFaultAlloc;
    unsigned long thpFaultFallback;
    unsigned long thpFaultFallbackCharge;
    unsigned long thpCollapseAlloc;
    unsigned long thpCollapseAllocFailed;
    unsigned long thpFileAlloc;
    unsigned long thpFileFallback;
    unsigned long thpFileFallbackCharge;
    unsigned long thpFileMapped;
    unsigned long thpSplitPage;
    unsigned long thpSplitPageFailed;
    unsigned long thpDeferredSplitPage;
    unsigned long thpSplitPmd;
    unsigned long thpScanExceedNonePte;
    unsigned long thpScanExceedSwapPte;
    unsigned long thpScanExceedSharePte;
    unsigned long thpSplitPud;
    unsigned long thpZeroPageAlloc;
    unsigned long thpZeroPageAllocFailed;
    unsigned long thpSwpout;
    unsigned long thpSwpoutFallback;

    // Ballooning
    unsigned long balloonInflate;
    unsigned long balloonDeflate;
    unsigned long balloonMigrate;

    // Swap readahead and hits
    unsigned long swapRa;
    unsigned long swapRaHit;

    // Kernel Samepage Merging (KSM)
    unsigned long ksmSwpinCopy;
    unsigned long cowKsm;

    // Zswap
    unsigned long zswpin;
    unsigned long zswpout;
    unsigned long zswpwb;

    // Direct map splits
    unsigned long directMapLevel2Splits;
    unsigned long directMapLevel3Splits;

    unsigned long nrUnstable;
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

void read_mem_vm_info(struct sMemVmInfo* mp);

char* get_mem_vm_data(struct sMemVmInfo* mp, const char* name);

struct memInfoStrings* get_all_mem_vm_data(struct sMemVmInfo* mp);

char* get_mem_info_data(struct sMemInfo* mi, const char* name);

struct memInfoStrings* get_all_mem_info_data(struct sMemInfo* mi);

void destroy_all_mem_data(struct memInfoStrings* ms);


const char** get_mem_info_names();

const char** get_mem_vm_names();

#endif //MEM_INFO_H
