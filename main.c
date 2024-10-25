#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "mem-info.h"
#include "mem-writer.h"
#include <argp.h>
#include <process-reader.h>

#include <sys/types.h>
#include <sys/wait.h>


const char *argp_program_version = "1.5";
const char *argp_program_bug_address = "<bug@example.com>";

static char doc[] = "Pull memory information";
static char args_doc[] = "-f <filename> -t <delay time>";

static struct argp_option options[] = {
    {"time", 't', 0, 0, "The time delay between reads"},
    {"file",  'f', "FILE", 0, "Output to filename to record the information"},
    {"process",  'p', "COMMAND", 0, "The command to execute and pull proc data for"},
    {0}
};

struct arguments {
    unsigned long time;
    char* filename;
    char* command;
    char** args;
};

static MemWriter* mw;


static error_t parse_opt(const int key, char *arg, const struct argp_state* state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 't':
            arguments->time = atoi(arg);
            break;
        case 'f':
            arguments->filename = arg;
            break;
        case 'p':
            arguments->command = arg;
            break;
        case ARGP_KEY_ARG:
            if (arguments->args == NULL) {
                arguments->args = malloc(sizeof(char *) * (state->arg_num + 2));
                arguments->args[0] = NULL;
            }
            arguments->args[state->arg_num] = arg;
            arguments->args[state->arg_num + 1] = NULL;
            break;
        case ARGP_KEY_END:
            if (arguments->command == NULL) {
                return 0;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};


void handle_signal(int sig) {

    // Check which signal was received and handle accordingly
    switch (sig) {
        case SIGTERM:
        case SIGINT:
        case SIGQUIT:
            printf("Finishing writing...\n");
            // TODO fix this, it deadlocks..
            // destroy_mem_writer(mw);
            printf("Exiting...\n");
            _exit(0);  // Exit the program
            break;
        default:
            printf("Received unknown signal (signal %d). Ignoring...\n", sig);
            break;
    }
}
int launch_process(struct arguments* args) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        char *exec_args[1024];
        exec_args[0] = args->command;

        for (int i = 0; args->args[i] != NULL; i++) {
            exec_args[i + 1] = args->args[i];
        }

        exec_args[1 + sizeof(args->args) / sizeof(args->args[0])] = NULL;

        execvp(exec_args[0], exec_args);

        perror("execvp");
        return -1;
    }else {
        printf("Executing %s (%d)..\n", args->command, pid);

        return pid;

    }
}

int main(int argc, char *argv[]){

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    struct arguments arguments;

    arguments.time = 10000;
    arguments.filename = "memlog.json";
    arguments.command = NULL;
    arguments.args = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);


    pid_t pid = -1;
    if (arguments.command != NULL) {
        pid = launch_process(&arguments);
        if (pid < 0) {
            return -1;
        }
    }

    struct sMemInfo* mi = malloc(sizeof(struct sMemInfo));
    struct sMemVmInfo* mp = malloc(sizeof(struct sMemVmInfo));
    struct sProcessInfo* pi = NULL;

    if (pid != -1) {
        pi = malloc(sizeof(struct sProcessInfo));
        init_process_info(pi, pid);
    }

    read_mem_info(mi);

    printf("Memory info:\n");
    printf(" - Total: %lu\n", mi->total);
    printf(" - Free: %lu\n", mi->free);
    printf(" - Available: %lu\n", mi->available);

    mw = new_mem_writer();

    init_mem_writer(mw, arguments.filename);

    printf("Writing memory info to file...\n");

    while (1) {
        read_mem_info(mi);
        read_mem_vm_info(mp);
        if (pid != -1) {
            read_process_info(pi);
        }
        write_mem(mw, mi, mp, pi);

        usleep(arguments.time);

        int status;
        const pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == -1) {
            perror("waitpid failed");
            break;
        }else if (result != 0) {
            if (WIFEXITED(status)) {
                printf("Child exited with status %d\n", WEXITSTATUS(status));
            } else {
                printf("Child terminated abnormally\n");
            }
            break;
        }

    }

    free(mi);
    free(mp);
    free(pi);


    return 0;
}
