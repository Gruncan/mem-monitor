#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem-info.h"
#include "mem-writer.h"
#include <argp.h>
#include <process-reader.h>

#include <sys/types.h>
#include <sys/wait.h>


#define READS_BEFORE 100


#ifndef MEM_DEBUG
#define GET_MEM_VERSION(v) #v
#else
#define GET_MEM_VERSION(v) #v "_d"
#endif


const char* argp_program_version = GET_MEM_VERSION(5.3p);

const char* argp_program_bug_address = "duncan.da.jones@gmail.com";

static char doc[] = "Pull memory information";
static char args_doc[] = "-f <filename> -t <delay time> -p <program to run>";

static struct argp_option options[] = {{"time", 't', "VALUE", 0, "The time delay between reads"},
                                       {"file", 'f', "FILE", 0, "Output to filename to record the information"},
                                       {"process", 'p', "COMMAND", 0, "The command to execute and pull proc data for"},
                                       {"processid", 'i', "ID", 0, "The existing process ID to monitor"},
                                       {"processname", 'n', "NAME", 0, "The existing process name to monitor"},
                                       {"proconly", 'o', "BOOL", 0, "Collect multiple process information override other keys"},
                                       {0}};

struct arguments {
    unsigned long time;
    char* filename;
    char* command;
    long int process_id;
    char* process_name;
    unsigned char is_proc_only;
    char** args;
    int is_collecting_args;
};

static MemWriter* mw;

static error_t parse_opt(const int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;

    switch (key) {
        case 't': {
            char* end_ptr;
            unsigned long value = strtoul(arg, &end_ptr, 10);
            if (*end_ptr != '\0') {
                printf("Invalid time: %s\n", end_ptr);
                return ARGP_KEY_ERROR;
            }
            arguments->time = value;
            break;
        }
        case 'f':
            arguments->filename = arg;
            break;
        case 'p':
            arguments->command = arg;
            arguments->is_collecting_args = 1;
            break;
        case 'i': {
            char* p_end_ptr;
            long int p_value = strtol(arg, &p_end_ptr, 10);
            if (*p_end_ptr != '\0') {
                printf("Invalid process id: %s\n", p_end_ptr);
                return ARGP_KEY_ERROR;
            }
            arguments->process_id = p_value;
            break;
        }
        case 'n':
            arguments->process_name = arg;
            break;
        case 'o':
            arguments->is_proc_only = 1;
            break;
        case ARGP_KEY_ARG:
            if (arguments->args == NULL) {
                arguments->args = malloc(sizeof(char*) * 2);
                if (arguments->args == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed.\n");
                    return ARGP_ERR_UNKNOWN;
                }
                arguments->args[0] = arg;
                arguments->args[1] = NULL;
            } else {
                int count = 0;
                while (arguments->args[count] != NULL) {
                    count++;
                }

                char** new_args = realloc(arguments->args, sizeof(char*) * (count + 2));
                if (new_args == NULL) {
                    fprintf(stderr, "Error: Memory reallocation failed.\n");
                    for (int i = 0; arguments->args[i] != NULL; i++) {
                        free(arguments->args[i]);
                    }
                    free(arguments->args);
                    arguments->args = NULL;
                    return ARGP_ERR_UNKNOWN;
                }

                arguments->args = new_args;
                arguments->args[count] = arg;
                arguments->args[count + 1] = NULL;
            }
            break;
        case ARGP_KEY_END:
            if (arguments->is_collecting_args && arguments->command == NULL) {
                return 0;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};


void handle_signal(const int sig) {
    // Check which signal was received and handle accordingly
    switch (sig) {
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            printf("Finishing writing...\n");
            // TODO fix this, it deadlocks..
            // destroy_mem_writer(mw);
            printf("Exiting...\n");
            _exit(0); // Exit the program
            break;
        default:
            printf("Received unknown signal (signal %d). Ignoring...\n", sig);
            break;
    }
}



static ProcessIds* construct_process_ids(const pid_t pid, const unsigned char is_proc_override) {
    ProcessIds* process_ids = malloc(sizeof(ProcessIds));
    if (process_ids == NULL) {
        perror("Error: Memory allocation failed.\n");
        return NULL;
    }
    init_process_ids(process_ids, &pid, 1, NULL, is_proc_override);

    return process_ids;
}


int main(int argc, char* argv[]) {
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    struct arguments arguments;

    arguments.time = 5000;
    arguments.filename = "temp.mtc";
    arguments.command = NULL;
    arguments.args = NULL;
    arguments.process_id = -1;
    arguments.process_name = NULL;
    arguments.is_proc_only = 0;
    arguments.is_collecting_args = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);


    ProcessIds* pids = NULL;
    if (arguments.command != NULL) {
        fprintf(stderr, "Forking of commands have been removed in latest version TBC!");
        return -1;
    } else if (arguments.process_id != -1) {
        pids = construct_process_ids(arguments.process_id, arguments.is_proc_only);
        if (pids == NULL) {
            return -1;
        }
    } else if (arguments.process_name != NULL) {
        pids = get_pids_by_name(arguments.process_name, arguments.is_collecting_args);
        if (pids == NULL) {
            return -1;
        }
    }

    mw = new_mem_writer();

    init_mem_writer(mw, arguments.filename);

    printf("Writing memory info to file...\n");

    while (1) {
        const unsigned char result = read_processes(pids);
        if (result == 1) {
            break; // All processes are dead
        }

        write_proc_mem(mw, pids);
        usleep(arguments.time);
    }

    free(pids);
    // free(mem_info);
    // free(mem_vm_info);

    return 0;
}
