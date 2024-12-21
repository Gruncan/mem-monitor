#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILE_NAME "memory_tracker.tmtc"


#ifdef __cplusplus
extern "C" {
#endif
static void* (*real_malloc) (size_t size) = NULL;
static void* (*real_calloc) (size_t nmemb, size_t size) = NULL;
static void* (*real_realloc) (void *ptr, size_t size) = NULL;
static void* (*real_reallocarray) (void* ptr, size_t nmemb, size_t size) = NULL;
static void  (*real_free) (void *ptr) = NULL;

#ifdef __cplusplus
}
#endif

static int log_fd;

void __attribute__((constructor)) lib_init() {
    log_fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    real_malloc       = (void* (*)(size_t))                dlsym(RTLD_NEXT, "malloc");
    real_calloc       = (void* (*)(size_t, size_t))        dlsym(RTLD_NEXT, "calloc");
    real_realloc      = (void* (*)(void*, size_t))         dlsym(RTLD_NEXT, "realloc");
    real_reallocarray = (void* (*)(void*, size_t, size_t)) dlsym(RTLD_NEXT, "reallocarray");
    real_free         = (void  (*)(void*))                 dlsym(RTLD_NEXT, "free");

    if (!real_malloc || !real_calloc || !real_realloc || !real_reallocarray || !real_free) {
        fprintf(stderr, "Failed to initialise memory tracker.\n");
    }

    const char* init_message = "Memory tracker initialized.\n";
    write(log_fd, init_message, strlen(init_message));
}

void __attribute__((destructor)) lib_cleanup() {
    const char* cleanup_message = "Memory tracker cleaning up.\n";
    write(log_fd, cleanup_message, strlen(cleanup_message));

    close(log_fd);
}

#ifdef __cplusplus
extern "C" {
#endif
void* malloc(size_t size) {
    void* ptr = real_malloc(size);
    char buffer[128];
    int len = snprintf(buffer, sizeof buffer, "malloc(%zd) = %p\n", size, ptr);
    write(log_fd, buffer, len);
    return ptr;
}

void* calloc(size_t nmemb, size_t size) {
    void* ptr = real_calloc(nmemb, size);
    char buffer[128];
    int len = snprintf(buffer, sizeof buffer, "calloc(%zd, %zd) = %p\n", nmemb, size, ptr);
    write(log_fd, buffer, len);
    return ptr;
}

void* reallocarray(void* ptr, size_t nmemb, size_t size) {
    void* new_ptr = real_reallocarray(ptr, nmemb, size);
    char buffer[128];
    int len = snprintf(buffer, sizeof buffer, "reallocarray(%p, %zd, %zd) = %p\n", ptr, nmemb, size, new_ptr);
    write(log_fd, buffer, len);
    return new_ptr;
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
#ifdef __cplusplus
}
#endif
