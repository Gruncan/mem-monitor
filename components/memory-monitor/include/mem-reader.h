/***********************************
 *
 * @brief The system wide profiling structs and reading function signatures
 *
 * @file mem-reader.h
 *
************************************/
#ifndef MEM_READER_H
#define MEM_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct __attribute__((packed)) mem_vm_info_s {
    // Zone-related stats
    unsigned long nrFreePages; // 0x0
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
    unsigned long numaHit; // 0xC
    unsigned long numaMiss;
    unsigned long numaForeign;
    unsigned long numaInterleave;
    unsigned long numaLocal;
    unsigned long numaOther;

    // Page usage
    unsigned long nrInactiveAnon; // 0x12
    unsigned long nrActiveAnon;
    unsigned long nrInactiveFile;
    unsigned long nrActiveFile;
    unsigned long nrUnevictable;
    unsigned long nrSlabReclaimable;
    unsigned long nrSlabUnreclaimable;
    unsigned long nrIsolatedAnon;
    unsigned long nrIsolatedFile;

    // Working set
    unsigned long workingsetNodes; // 0x1B
    unsigned long workingsetRefaultAnon;
    unsigned long workingsetRefaultFile;
    unsigned long workingsetActivateAnon;
    unsigned long workingsetActivateFile;
    unsigned long workingsetRestoreAnon;
    unsigned long workingsetRestoreFile;
    unsigned long workingsetNodereclaim;

    // Anonymous and file pages
    unsigned long nrAnonPages; // 0x23
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
    unsigned long nrDirtyThreshold; // 0x2F
    unsigned long nrDirtyBackgroundThreshold;

    // Page reclaim and fault
    unsigned long pgpgin; // 0x31
    unsigned long pgpgout;
    unsigned long pswpin;
    unsigned long pswpout;
    unsigned long pgfault;
    unsigned long pgmajfault;

    // Page allocation
    unsigned long pgallocDma; // 0x37
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
    unsigned long pgstealKswapd; // 0x4D
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
    unsigned long zoneReclaimFailed; // 0x58
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
    unsigned long numaPteUpdates; // 0x63
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
    unsigned long compactMigrateScanned; // 0x6D
    unsigned long compactFreeScanned;
    unsigned long compactIsolated;
    unsigned long compactStall;
    unsigned long compactFail;
    unsigned long compactSuccess;
    unsigned long compactDaemonWake;
    unsigned long compactDaemonMigrateScanned;
    unsigned long compactDaemonFreeScanned;

    // High-order allocations
    unsigned long htlbBuddyAllocSuccess; // 0x76
    unsigned long htlbBuddyAllocFail;

    // Unevictable pages
    unsigned long unevictablePgsCulled; // 0x78
    unsigned long unevictablePgsScanned;
    unsigned long unevictablePgsRescued;
    unsigned long unevictablePgsMlocked;
    unsigned long unevictablePgsMunlocked;
    unsigned long unevictablePgsCleared;
    unsigned long unevictablePgsStranded;

    // Transparent hugepage stats
    unsigned long thpFaultAlloc; // 0x7F
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
    unsigned long balloonInflate; // 0x94
    unsigned long balloonDeflate;
    unsigned long balloonMigrate;

    // Swap readahead and hits
    unsigned long swapRa; // 0x97
    unsigned long swapRaHit;

    // Kernel Samepage Merging (KSM)
    unsigned long ksmSwpinCopy; // 0x99
    unsigned long cowKsm;

    // Zswap
    unsigned long zswpin; // 0x9b
    unsigned long zswpout;
    unsigned long zswpwb;

    // Direct map splits
    unsigned long directMapLevel2Splits; // 0x9e
    unsigned long directMapLevel3Splits;

    unsigned long nrUnstable; // 0xa0
} MemVmInfo;


typedef struct __attribute__((packed)) mem_info_s {
    unsigned long total;
    unsigned long free;
    unsigned long available;
    unsigned long buffers;
    unsigned long cached;
    unsigned long swapCache;

    unsigned long active; // 0xa7
    unsigned long inActive;
    unsigned long activeAnon;
    unsigned long inActiveAnon;
    unsigned long activeFile;
    unsigned long inActiveFile;

    unsigned long unevictable;
    unsigned long mLocked;

    unsigned long swapTotal; // 0xaf
    unsigned long swapFree;
    unsigned long zswap;
    unsigned long zswapped;

    unsigned long dirty;
    unsigned long writeback;

    unsigned long pagesAnon; // 0xb5
    unsigned long pageMapped;

    unsigned long shmem;

    unsigned long kreClaimable; // 0xb8
    unsigned long slab;
    unsigned long srClaimable;
    unsigned long sunReclaim;

    unsigned long kernelStack; // 0xbc
    unsigned long pageTables;
    unsigned long secPageTables;
    unsigned long nfsUnstable;
    unsigned long bounce;
    unsigned long writebackTmp;

    unsigned long commitLimit;
    unsigned long committedAllocs;

    unsigned long vmallocTotal; // 0xc4
    unsigned long vmallocUsed;
    unsigned long vmallocChunk;

    unsigned long perCPU;
    unsigned long hardwareCorrupted;

    unsigned long hugePagesAnon; // 0xc9
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

    unsigned long directMap4k; // 0xd5
    unsigned long directMap2M;
    unsigned long directMap1G;
} MemInfo;


void read_mem_info(MemInfo* mem_info);

void read_mem_vm_info(MemVmInfo* mem_vm_info);


#endif // MEM_READER_H
