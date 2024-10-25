#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "mem-info.h"
#include "mem-writer.h"
#include <argp.h>


const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "<bug@example.com>";

static char doc[] = "Pull memory information";
static char args_doc[] = "-f <filename> -t <delay time>";

static struct argp_option options[] = {
    {"time", 't', 0, 0, "The time delay between reads"},
    {"file",  'f', "FILE", 0, "Output to filename to record the information"},
    {0}
};

struct arguments {
    unsigned long time;
    char* filename;
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


int main(int argc, char *argv[]){

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    struct arguments arguments;

    arguments.time = 10000;
    arguments.filename = "memlog.json";

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    struct sMemInfo* mi = malloc(sizeof(struct sMemInfo));
    struct sMemVmInfo* mp = malloc(sizeof(struct sMemVmInfo));

    read_mem_info(mi);
    read_mem_vm_info(mp);

    printf("Memory info:\n");
    printf(" - Total: %lu\n", mi->total);
    printf(" - Free: %lu\n", mi->free);
    printf(" - Available: %lu\n", mi->available);

    printf("\nPage info:\n");
    printf(" - pgfault: %lu\n", mp->pgfault);
    printf(" - pgmajfault: %lu\n", mp->pgmajfault);
    printf(" - pgpgin: %lu\n", mp->pgpgin);
    printf(" - pgpgout: %lu\n", mp->pgpgout);

    mw = new_mem_writer();

    init_mem_writer(mw, arguments.filename);

    printf("Writing memory info to file...\n");

    while (1) {
        read_mem_info(mi);
        read_mem_vm_info(mp);
        write_mem(mw, mi, mp);


        usleep(arguments.time);
    }


    free(mi);
    free(mp);

    return 0;
}
