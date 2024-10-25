
#include "mem-info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096  // 4Kb. This should be more than enough for big systems,
                          //        might need to tweek for lower systems..

static const size_t sizeUL = sizeof(unsigned long);

static const char* memMapping[] = {
    "MemTotal", "MemFree", "MemAvailable", "Buffers", "Cached", "SwapCached",
    "Active"," Inactive", "Active(anon)", "Inactive(anon)","Active(file)",
    "Inactive(file)", "Unevictable", "Mlocked", "SwapTotal", "SwapFree", "Zswap",
    "Zswapped", "Dirty", "Writeback", "AnonPages", "Mapped", "Shmem", "KReclaimable",
    "Slab", "SReclaimable", "SUnreclaim", "KernelStack", "PageTables", "SecPageTables",
    "NFS_Unstable", "Bounce", "WritebackTmp", "CommitLimit", "Committed_AS", "VmallocTotal",
    "VmallocUsed", "VmallocChunk", "Percpu", "HardwareCorrupt", "AnonHugePages",
    "ShmemHugePages", "ShmemPmdMapped", "FileHugePages", "FilePmdMapped", "Unaccepted",
    "HugePages_Total", "HugePages_Free", "HugePages_Rsvd", "HugePages_Surp", "Hugepagesize",
    "Hugetlb", "DirectMap4k", "DirectMap2M", "DirectMap1G",
};

static const char* memPageMapping[] = {
    "nr_free_pages", "nr_zone_inactive_anon", "nr_zone_active_anon", "nr_zone_inactive_file", "nr_zone_active_file",
    "nr_zone_unevictable", "nr_zone_write_pending", "nr_mlock", "nr_bounce", "nr_zspages", "nr_free_cma", "nr_unaccepted",
    "numa_hit", "numa_miss", "numa_foreign", "numa_interleave", "numa_local", "numa_other",
    "nr_inactive_anon", "nr_active_anon", "nr_inactive_file", "nr_active_file", "nr_unevictable", "nr_slab_reclaimable",
    "nr_slab_unreclaimable", "nr_isolated_anon", "nr_isolated_file",
    "workingset_nodes", "workingset_refault_anon", "workingset_refault_file", "workingset_activate_anon",
    "workingset_activate_file", "workingset_restore_anon", "workingset_restore_file", "workingset_nodereclaim",
    "nr_anon_pages", "nr_mapped", "nr_file_pages", "nr_dirty", "nr_writeback", "nr_writeback_temp", "nr_shmem",
    "nr_shmem_hugepages", "nr_shmem_pmdmapped", "nr_file_hugepages", "nr_file_pmdmapped", "nr_anon_transparent_hugepages",
    "nr_dirty_threshold", "nr_dirty_background_threshold", "pgpgin", "pgpgout", "pswpin", "pswpout", "pgfault", "pgmajfault",
    "pgalloc_dma", "pgalloc_dma32", "pgalloc_normal", "pgalloc_movable", "pgalloc_device", "allocstall_dma", "allocstall_dma32",
    "allocstall_normal", "allocstall_movable", "allocstall_device", "pgskip_dma", "pgskip_dma32", "pgskip_normal", "pgskip_movable",
    "pgskip_device", "pgfree", "pgactivate", "pgdeactivate", "pglazyfree", "pglazyfreed", "pgrefill", "pgreuse", "pgsteal_kswapd",
    "pgsteal_direct", "pgsteal_khugepaged", "pgscan_kswapd", "pgscan_direct", "pgscan_khugepaged", "pgscan_direct_throttle",
    "pgscan_anon", "pgscan_file", "pgsteal_anon", "pgsteal_file", "zone_reclaim_failed", "pginodesteal", "slabs_scanned",
    "kswapd_inodesteal", "kswapd_low_wmark_hit_quickly", "kswapd_high_wmark_hit_quickly", "pageoutrun", "pgrotated",
    "drop_pagecache", "drop_slab", "oom_kill", "numa_pte_updates", "numa_huge_pte_updates", "numa_hint_faults",
    "numa_hint_faults_local", "numa_pages_migrated", "pgmigrate_success", "pgmigrate_fail", "thp_migration_success",
    "thp_migration_fail", "thp_migration_split", "compact_migrate_scanned", "compact_free_scanned", "compact_isolated",
    "compact_stall", "compact_fail", "compact_success", "compact_daemon_wake", "compact_daemon_migrate_scanned",
    "compact_daemon_free_scanned", "htlb_buddy_alloc_success", "htlb_buddy_alloc_fail", "unevictable_pgs_culled",
    "unevictable_pgs_scanned", "unevictable_pgs_rescued", "unevictable_pgs_mlocked", "unevictable_pgs_munlocked",
    "unevictable_pgs_cleared", "unevictable_pgs_stranded", "thp_fault_alloc", "thp_fault_fallback", "thp_fault_fallback_charge",
    "thp_collapse_alloc", "thp_collapse_alloc_failed", "thp_file_alloc", "thp_file_fallback", "thp_file_fallback_charge",
    "thp_file_mapped", "thp_split_page", "thp_split_page_failed", "thp_deferred_split_page", "thp_split_pmd",
    "thp_scan_exceed_none_pte", "thp_scan_exceed_swap_pte", "thp_scan_exceed_share_pte", "thp_split_pud", "thp_zero_page_alloc",
    "thp_zero_page_alloc_failed", "thp_swpout", "thp_swpout_fallback", "balloon_inflate", "balloon_deflate", "balloon_migrate",
    "swap_ra", "swap_ra_hit", "ksm_swpin_copy", "cow_ksm", "zswpin", "zswpout", "zswpwb", "direct_map_level2_splits",
    "direct_map_level3_splits", "nr_unstable"
};


static const char* memInfoNames[] = {
    "total", "free", "available", "buffers", "cached", "swapCache", "active", "inActive", "activeAnon", "inActiveAnon",
    "activeFile", "inActiveFile", "unevictable", "mLocked", "swapTotal", "swapFree",
    "zswap", "zswapped", "dirty", "writeback", "pagesAnon", "pageMapped", "shmem", "kreClaimable", "slab",
    "srClaimable",
    "sunReclaim", "kernelStack", "pageTables", "secPageTables", "nfsUnstable", "bounce", "writebackTmp", "commitLimit",
    "committedAllocs", "vmallocTotal", "vmallocUsed", "vmallocChunk", "perCPU", "hardwareCorrupted", "hugePagesAnon",
    "hugePagesShmem", "pmdMappedShmem", "hugePagesFile", "pmdMappedFile", "unaccepted", "hugePagesTotal",
    "hugePagesFree",
    "hugePagesRsvd", "hugePagesSurp", "hugePageSize", "hugePageTLB", "directMap4k", "directMap2M", "directMap1G"
};



char* get_ulong_str(const unsigned long ulong_value) {
    const int length = snprintf(NULL, 0, "%lu", ulong_value);

    char *buf = malloc(length + 1);
    if (buf == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    snprintf(buf, length + 1, "%lu", ulong_value);

    return buf;
}


void set_mem_struct_value(void* sStruct, const size_t structLength,
                          const char* map[], const char* key, const unsigned long value) {
    for (int i = 0; i < structLength / sizeUL; i++) {
        if(strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * sizeUL;
            if (valueOffset >= structLength) {
                perror("Failed to write to struct!");
                return;
            }
            // This looks disgusting.. I thought I understood C but pointer arithmetic needs casting to 1 byte char?
            *(unsigned long*)((char*)sStruct + valueOffset) = value;

            break;
        }
    }
}

unsigned long get_mem_struct_value(void* sStruct, const size_t structLength, const char* map[], const char* key) {
    for (int i = 0; i < structLength / sizeUL; i++) {
        if (strcmp(key, map[i]) == 0) {
            const size_t valueOffset = i * sizeUL;
            if (valueOffset >= structLength) {
                perror("Failed to read from struct!");
            }
            return *(unsigned long*) ((char*) sStruct + valueOffset);
        }
    }

    return -1; // Error memory stat should not be possible for negative?
}

struct memInfoStrings* get_all_mem_struct_values(const unsigned long* values, const size_t valuesLength) {
    char** buffer = malloc(valuesLength * sizeof(char*));
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    for (int i = 0; i < valuesLength / sizeUL; i++) {
        unsigned long v = *(unsigned long*) ((char*) values + (i * sizeUL));
        buffer[i] = get_ulong_str(v);
    }

    struct memInfoStrings* ms = malloc(sizeof(struct memInfoStrings));

    ms->mem_strings = buffer;
    ms->mem_strings_count = valuesLength / sizeUL;

    return ms;
}



void mem_parse_page_line(const char* line, struct sMemVmInfo* mp) {
    char key[256];
    unsigned long value;
    int items = sscanf(line, "%255s %lu", key, &value);
    if (items != 2) {
        // Something has gone wrong..
        return;
    }

    char* newline = strchr(key, '\n');
    if (newline) *newline = '\0';

    const size_t structLength = sizeof(struct sMemVmInfo);

    set_mem_struct_value(mp, structLength, memPageMapping, key, value);

}

void mem_parse_line(const char* line, struct sMemInfo* mi) {
    char key[256];
    unsigned long value;
    char unit[32];

    int items = sscanf(line, "%255[^:]: %lu %31s", key, &value, unit);
    if (items < 2) {
        items = sscanf(line, "%255[^:]: %lu", key, &value);
    }

    if (items < 2) {
        // We should always have a unit?
        return;
    }

    char* newline = strchr(key, '\n');
    if (newline) *newline = '\0';

    char* paren = strchr(key, '(');
    if (paren) *paren = '\0';

    const size_t structLength = sizeof(struct sMemInfo);

    set_mem_struct_value(mi, structLength, memMapping, key, value);

}


char* mem_parse_file(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }


    size_t content_size = 0;
    char* content = malloc(BUFFER_SIZE);
    if (content == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    size_t bytes_read;
    while ((bytes_read = fread(content + content_size, 1, BUFFER_SIZE - content_size - 1, fp)) > 0) {
        content_size += bytes_read;
    }
    content[content_size] = '\0';

    fclose(fp);

    return content;
}


void read_mem_info(struct sMemInfo* mi) {
    char* content = mem_parse_file("/proc/meminfo");
    if (content == NULL) {
        perror("Failed to access /proc/meminfo");
        return;
    }

    char* line = strtok(content, "\n");
    while (line != NULL) {
        mem_parse_line(line, mi);
        line = strtok(NULL, "\n");
    }

    free(content);

}

void read_mem_vm_info(struct sMemVmInfo* mp) {
    char* content = mem_parse_file("/proc/vmstat");
    if (content == NULL) {
        perror("Failed to access /proc/vmstat");
        return;
    }

    char *line = strtok(content, "\n");
    while (line != NULL) {
        mem_parse_page_line(line, mp);
        line = strtok(NULL, "\n");
    }

    free(content);
}


char* get_mem_vm_data(struct sMemVmInfo* mp, const char *name) {
    const unsigned long value = get_mem_struct_value(mp, sizeof(struct sMemVmInfo), memPageMapping, name);
    if (value < 0) {
        return NULL;
    }

    return get_ulong_str(value);
}

struct memInfoStrings* get_all_mem_vm_data(struct sMemVmInfo* mp) {
    return get_all_mem_struct_values((unsigned long*) mp, sizeof(struct sMemVmInfo));
}

char* get_mem_info_data(struct sMemInfo* mi, const char* name) {
    const unsigned long value = get_mem_struct_value(mi, sizeof(struct sMemInfo), memInfoNames, name);
    if (value < 0) {
        return NULL;
    }

    return get_ulong_str(value);
}

struct memInfoStrings* get_all_mem_info_data(struct sMemInfo* mi) {
    return get_all_mem_struct_values((unsigned long*) mi, sizeof(struct sMemInfo));
}

void destroy_all_mem_data(struct memInfoStrings* ms) {
    for (int i=0; i < ms->mem_strings_count; i++) {
        free(ms->mem_strings[i]);
    }
    free(ms->mem_strings);
    free(ms);

}

const char** get_mem_info_names() {
    return memInfoNames;
}

const char** get_mem_vm_names() {
    return memPageMapping;
}
