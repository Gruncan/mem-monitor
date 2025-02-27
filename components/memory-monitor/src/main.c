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
#define GET_MEM_VERSION(v) #v "d"
#endif


const char* argp_program_version = GET_MEM_VERSION(4.2);

const char* argp_program_bug_address = "duncan.da.jones@gmail.com";

static char doc[] = "Pull memory information";
static char args_doc[] = "-f <filename> -t <delay time> -p <program to run>";

static struct argp_option options[] = {{"time", 't', "VALUE", 0, "The time delay between reads"},
                                       {"file", 'f', "FILE", 0, "Output to filename to record the information"},
                                       {"process", 'p', "COMMAND", 0, "The command to execute and pull proc data for"},
                                       {"processid", 'i', "ID", 0, "The existing process ID to monitor"},
                                       {"processname", 'n', "NAME", 0, "The existing process name to monitor"},
                                       {0}};

struct arguments {
    unsigned long time;
    char* filename;
    char* command;
    long int process_id;
    char* process_name;
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

pid_t get_pid_by_name(const char* name) {
    char command[256];
    // pgrep is probably more efficient that what i would do.. and quicker for me :)
    snprintf(command, sizeof(command), "pgrep %s", name);

    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        return -1;
    }

    pid_t pid = -1;
    if (fscanf(fp, "%d", &pid) != 1) {
        printf("No process found with name: %s\n", name);
        fclose(fp);
        return -2;
    }

    fclose(fp);
    return pid;
}

int launch_process(struct arguments* args) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        char* exec_args[1024];
        exec_args[0] = args->command;

        int i = 0;
        while (args->args[i] != NULL) {
            exec_args[i + 1] = args->args[i];
            i++;
        }
        exec_args[i + 1] = NULL;

        execvp(exec_args[0], exec_args);

        perror("execvp");
        return -1;
    } else {
        printf("Executing %s (%d)..\n", args->command, pid);

        return pid;
    }
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
    arguments.is_collecting_args = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    int is_child_proc = 0;

    pid_t pid = -1;
    if (arguments.command != NULL) {
        pid = launch_process(&arguments);
        if (pid <= 0) {
            return -1;
        }
        is_child_proc = 1;
    } else if (arguments.process_id != -1) {
        pid = (pid_t) arguments.process_id;
    } else if (arguments.process_name != NULL) {
        pid = get_pid_by_name(arguments.process_name);
    }


    MemInfo* mem_info = malloc(sizeof(MemInfo));
    MemVmInfo* mem_vm_info = malloc(sizeof(MemVmInfo));
    MemProcInfo* mem_proc_info = NULL;

    if (pid == -2) {
        return -1;
    }
    if (pid != -1) {
        mem_proc_info = malloc(sizeof(MemProcInfo));
        if (init_process_info(mem_proc_info, pid) == -1) {
            return -1;
        }
    }

    read_mem_info(mem_info);


    printf("Memory info:\n");
    printf(" - Total: %lu\n", mem_info->total);
    printf(" - Free: %lu\n", mem_info->free);
    printf(" - Available: %lu\n", mem_info->available);

    mw = new_mem_writer();

    init_mem_writer(mw, arguments.filename);

    printf("Writing memory info to file...\n");

    int process_terminated = 0;
    int counter = 0;

    while (1) {
        read_mem_info(mem_info);
        read_mem_vm_info(mem_vm_info);
        if (pid != -1) {
            const char result = read_process_info(mem_proc_info, pid);
            if (result < 0)
                pid = -1;
        }
        write_mem(mw, mem_info, mem_vm_info, mem_proc_info);

        usleep(arguments.time);

        if (counter >= READS_BEFORE) {
            break;
        } else if (process_terminated == 1) {
            counter++;
        } else if (pid != -1) {
            int result;
            int status;
            if (is_child_proc == 1) {
                result = waitpid(pid, &status, WNOHANG);
            } else {
                result = check_process_exists(pid);
                if (result == 1) {
                    result = 0;
                } else if (result == 0) {
                    result = 1;
                }
                status = 0;
            }

            if (result == -1) {
                perror("failed to check process status");
                break;
            }

            if (result != 0) {
                if (is_child_proc != 1) {
                    printf("Unsure exit status! %d\n", result);
                } else if (WIFEXITED(status)) {
                    printf("Child exited with status %d\n", WEXITSTATUS(status));
                } else if (WIFSTOPPED(status)) {
                    printf("Child stopped by signal %d\n", WSTOPSIG(status));
                } else {
                    printf("Process exited with unknown status: %d\n", status);
                }
                printf("Collecting %d more data points..\n", READS_BEFORE);
                pid = -1;
                reset_process_info(mem_proc_info);
                process_terminated = 1;
            }
        }
    }

    free(mem_info);
    free(mem_vm_info);
    if (mem_proc_info != NULL) {
        free(mem_proc_info);
    }

    return 0;
}
