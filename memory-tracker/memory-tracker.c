#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static void* (*real_malloc) (size_t size) = NULL;
static void* (*real_realloc) (void *ptr, size_t size) = NULL;
static void  (*real_free) (void *ptr) = NULL;

static int log_fd;

void __attribute__((constructor)) lib_init() {
    log_fd = open("memory_tracker.log", O_WRONLY | O_CREAT | O_APPEND, 0644);

    real_malloc  = (void* (*)(size_t)) dlsym(RTLD_NEXT, "malloc");
    real_realloc = (void* (*)(void*, size_t)) dlsym(RTLD_NEXT, "realloc");
    real_free    = (void (*)(void*)) dlsym(RTLD_NEXT, "free");


    if (!real_malloc || !real_realloc || !real_free) {
        fprintf(stderr, "Failed to initalise memory tracker.\n");
    }

    const char* init_message = "Memory tracker initialized.\n";
    write(log_fd, init_message, strlen(init_message));
}

void __attribute__((destructor)) lib_cleanup() {
    const char* cleanup_message = "Memory tracker cleaning up.\n";
    write(log_fd, cleanup_message, strlen(cleanup_message));

    close(log_fd);
}

void* malloc(size_t size) {
    void* ptr = real_malloc(size);
    char buffer[128];
    int len = snprintf(buffer, sizeof buffer, "malloc(%zd) = %p\n", size, ptr);
    write(log_fd, buffer, len);
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    void* new_ptr = real_realloc(ptr, size);
    char buffer[128];
    int len = snprintf(buffer, sizeof buffer, "realloc(%p, %zd) = %p\n", ptr, size, new_ptr);
    write(log_fd, buffer, len);
    return new_ptr;
}

void free(void *ptr) {
    real_free(ptr);
    char buffer[128];
    int len = snprintf(buffer, sizeof buffer, "free(%p)\n", ptr);
    write(log_fd, buffer, len);
}