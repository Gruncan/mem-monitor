
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main(){
    int v = sysconf(_SC_PAGESIZE);
    printf("v: %d\n", v);
}