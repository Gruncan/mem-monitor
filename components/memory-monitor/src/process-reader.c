

#include "process-reader.h"

#include <errno.h>
#include <mem-info.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define STATM_FIELDS 6



ProcessIds* get_pids_by_name(char* name, unsigned char is_proc_override) {
    char command[256];
    // pgrep is probably more efficient that what i would do.. and quicker for me :)
    snprintf(command, sizeof(command), "pgrep %s", name);

    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen");
        return NULL;
    }

    size_t size = 0;
    pid_t* pids = malloc(sizeof(pid_t));
    if (pids == NULL) {
        perror("Failed failed to allocate memory for process ids");
        return NULL;
    }

    char line[32];
    int i = 0;
    while (fgets(line, sizeof(line), pipe) != NULL) {
        const size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        if (size != 0 && i == size) {
            size *= 2;
            pid_t* new_pids = realloc(pids, sizeof(pid_t) * size);
            if (new_pids == NULL) {
                perror("Failed failed to allocate memory for pids");
                goto cleanUpFunc;
            }
            pids = new_pids;
        }

        pids[i] = atoi(line);
        if (size == 0) size = 1;

        i++;
    }


    ProcessIds* process_ids = malloc(sizeof(ProcessIds));
    if (process_ids == NULL) {
        perror("Failed failed to allocate memory for process ids");
        goto cleanUpFunc;
    }else if (size == 0) {
        free(pids);
        process_ids->pids = NULL;
        process_ids->size = 0;
        process_ids->name = name;
        process_ids->is_proc_override = is_proc_override;
        return process_ids;
    }

    pid_t* shrunk_pids = realloc(pids, sizeof(pid_t) * i);
    if (shrunk_pids == NULL) {
        perror("Failed failed to allocate memory for pids");
        goto cleanUpFunc;
    }

    pclose(pipe);
    process_ids->size = i;
    process_ids->pids = shrunk_pids;
    process_ids->name = name;
    process_ids->is_proc_override = is_proc_override;
    return process_ids;

cleanUpFunc:
    free(pids);
    pclose(pipe);
    return NULL;
}


int check_process_exists(pid_t pid) {
    if (kill(pid, 0) == 0)
        return 1;

    if (errno == ESRCH)
        return 0;


    perror("kill");
    return -1;
}

int init_process_info(MemProcInfo* mem_proc_info, pid_t pid) {
    if (check_process_exists(pid) == 0) {
        perror("Process does not exist");
        return -1;
    }

    mem_proc_info->oom_adj = -1;
    mem_proc_info->oom_score = -1;
    mem_proc_info->oom_score_adj = -1;
    mem_proc_info->data = 0;
    mem_proc_info->size = 0;
    mem_proc_info->resident = 0;
    mem_proc_info->shared = 0;
    mem_proc_info->text = 0;
    mem_proc_info->data = 0;
    mem_proc_info->dirty = 0;

    return 0;
}


void read_process_mem_info(MemProcInfo* mem_proc_info, const pid_t pid) {
    char file[256];
    snprintf(file, sizeof(file), "/proc/%d/statm", pid);

    char* content = mem_parse_file(file, 64, READ_BINARY);
    if (content == NULL) {
        printf("mem_parse_file failed\n");
        return;
    }

    if (strlen(content) == 0) {
        mem_proc_info->size = 0;
        mem_proc_info->resident = 0;
        mem_proc_info->shared = 0;
        mem_proc_info->text = 0;
        mem_proc_info->data = 0;
        mem_proc_info->dirty = 0;
    } else {
        if (sscanf(content, "%lu %lu %lu %lu %lu %lu", &mem_proc_info->size, &mem_proc_info->resident,
                   &mem_proc_info->shared, &mem_proc_info->text, &mem_proc_info->data,
                   &mem_proc_info->dirty) != STATM_FIELDS) {
            perror("Failed to parse /proc/pid/statm");
            free(content);
            return;
        }

        const long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;

        mem_proc_info->size *= page_size_kb;
        mem_proc_info->resident *= page_size_kb;
        mem_proc_info->shared *= page_size_kb;
        mem_proc_info->text *= page_size_kb;
        mem_proc_info->data *= page_size_kb;
        mem_proc_info->dirty *= page_size_kb;
    }

    free(content);
}

char read_process_info(MemProcInfo* mem_proc_info, const pid_t pid) {
    if (mem_proc_info == NULL)
        return -1;

    const size_t length = 3;

    char* files[] = {"/proc/%d/oom_adj", "/proc/%d/oom_score", "/proc/%d/oom_score_adj"};

    char filenames[length][30];

    char directory[20];
    sprintf(directory, "/proc/%d", pid);

    struct stat stats;
    if (stat(directory, &stats) == 0) {
        if (!S_ISDIR(stats.st_mode)) {
            reset_process_info(mem_proc_info);
            return -1;
        }
    }

    for (int i = 0; i < length; i++) {
        sprintf(filenames[i], files[i], pid);
        char* content = mem_parse_file(filenames[i], 8, READ_RAW);
        if (content == NULL) break;
        const int value = atoi(content);
        switch (i) {
            case 0:
                mem_proc_info->oom_adj = value;
                break;
            case 1:
                mem_proc_info->oom_score = value;
                break;
            case 2:
                mem_proc_info->oom_score_adj = value;
                break;
            default:
                break;
        }
        free(content);
    }

    read_process_mem_info(mem_proc_info, pid);
    return 0;
}

void reset_process_info(MemProcInfo* mem_proc_info) {
    if (mem_proc_info == NULL)
        return;

    mem_proc_info->size = -1;
    mem_proc_info->resident = -1;
    mem_proc_info->shared = -1;
    mem_proc_info->text = -1;
    mem_proc_info->data = -1;
    mem_proc_info->dirty = -1;
}
