/*********************************
 *
 *
 *
 *********************************/
#define _GNU_SOURCE

#ifdef __cplusplus
#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <type_traits>

#define NONE nullptr

#else
#include <stdio.h>
#include <string.h>

#define NONE NULL

#endif

#include <bits/stdint-uintn.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include "mem-monitor-config.h"


#define SYMBOL_NEW "_Znwm"
#define SYMBOL_NEW_NOTHROW "_ZnwmRKSt9nothrow_t"
#define SYMBOL_DELETE "_ZdlPv"
#define SYMBOL_DELETE_SIZED "_ZdlPvm"
#define SYMBOL_DELETE_NOTHROW "_ZdlPvRKSt9nothrow_t"
#define SYMBOL_NEW_ARRAY "_Znam"
#define SYMBOL_NEW_ARRAY_NOTHROW "_ZnamRKSt9nothrow_t"
#define SYMBOL_DELETE_ARRAY "_ZdaPv"
#define SYMBOL_DELETE_ARRAY_SIZED "_ZdaPvm"
#define SYMBOL_DELETE_ARRAY_NOTHROW "_ZdaPvRKSt9nothrow_t"
// C++ 17 specifics
#define SYMBOL_NEW_ALIGN "_ZnwmSt11align_val_t"
#define SYMBOL_NEW_ARRAY_ALIGN "_ZnamSt11align_val_t"
#define SYMBOL_DELETE_ALIGN "_ZdlPvSt11align_val_t"
#define SYMBOL_DELETE_ARRAY_ALIGN "_ZdaPvSt11align_val_t"
#define SYMBOL_NEW_PLACEMENT "_ZnwmPv"           // Unused
#define SYMBOL_NEW_ARRAY_PLACEMENT "_ZnamPv"     // Unused
#define SYMBOL_DELETE_PLACEMENT "_ZdlPvS_"       // Unused
#define SYMBOL_DELETE_ARRAY_PLACEMENT "_ZdaPvS_" // Unused


#define MALLOC 0x0
#define CALLOC 0x1
#define REALLOC 0x2
#define REALLOC_ARRAY 0x3
#define FREE 0x4

#define NEW 0x5
#define NEW_NOTHROW 0x6
#define NEW_ARRAY 0x7
#define NEW_ARRAY_NOTHROW 0x8

#define DELETE 0x9
#define DELETE_SIZED 0xa
#define DELETE_NOTHROW 0xb
#define DELETE_ARRAY 0xc
#define DELETE_ARRAY_SIZED 0xd
#define DELETE_ARRAY_NOTHROW 0xe

#define NEW_ALIGN 0xf
#define NEW_ARRAY_ALIGN 0x10
#define DELETE_ALIGN 0x11
#define DELETE_ARRAY_ALIGN 0x12


#define CPP_17 201703L

#ifndef FILE_PATH
    #define FILE_PATH "memory_tracker1.tmtc"
#endif

#ifndef FILE_INDEX
    #define FILE_INDEX 14
#endif

#define ERROR_PREFIX "[MEMORY_TRACKER] ERROR: "
#define DEBUG_PREFIX "[MEMORY_TRACKER] DEBUG: "

#define LOAD_SYMBOL(symbol, funcType)                                                                                  \
    if (real_##symbol == NONE) {                                                                                       \
        real_##symbol = (funcType) dlsym(RTLD_NEXT, #symbol);                                                          \
    }

#define CONVERT_ALIGN(align) static_cast<std::underlying_type_t<std::align_val_t>>(align)

static void* (*real_malloc)(size_t size) = NONE;
static void* (*real_calloc)(size_t nmemb, size_t size) = NONE;
static void* (*real_realloc)(void* ptr, size_t size) = NONE;
static void* (*real_reallocarray)(void* ptr, size_t nmemb, size_t size) = NONE;
static void (*real_free)(void* ptr) = NONE;

#ifdef __cplusplus
using NewFuncType = void* (*) (std::size_t);
static NewFuncType real_new = NONE;
using NewFuncNoThrowType = void* (*) (std::size_t, const std::nothrow_t&) noexcept;
static NewFuncNoThrowType real_new_nothrow = NONE;

using DeleteFuncType = void (*)(void*);
static DeleteFuncType real_delete = NONE;
using DeleteFuncSizedType = void (*)(void*, std::size_t);
static DeleteFuncSizedType real_delete_sized = NONE;
using DeleteFuncNoThrowType = void (*)(void*, const std::nothrow_t&) noexcept;
static DeleteFuncNoThrowType real_delete_nothrow = NONE;

using NewArrayFuncType = void* (*) (std::size_t);
static NewArrayFuncType real_new_array = NONE;
using NewArrayFuncNoThrowType = void* (*) (std::size_t, const std::nothrow_t&) noexcept;
static NewArrayFuncNoThrowType real_new_array_nothrow = NONE;

using DeleteArrayFuncType = void (*)(void*) noexcept;
static DeleteArrayFuncType real_delete_array = NONE;
using DeleteArrayFuncSizedType = void (*)(void*, std::size_t) noexcept;
static DeleteArrayFuncSizedType real_delete_array_sized = NONE;
using DeleteArrayFuncNoThrowType = void (*)(void*, const std::nothrow_t&) noexcept;
static DeleteArrayFuncNoThrowType real_delete_array_nothrow = NONE;

#if __cplusplus >= CPP_17

using NewAlignFuncType = void* (*) (std::size_t, std::align_val_t);
static NewAlignFuncType real_new_align = NONE;
using NewArrayAlignFuncType = void* (*) (std::size_t, std::align_val_t);
static NewArrayAlignFuncType real_new_array_align = NONE;

using DeleteAlignFuncType = void (*)(void*, std::align_val_t) noexcept;
static DeleteAlignFuncType real_delete_align = NONE;
using DeleteArrayAlignFuncType = void (*)(void*, std::align_val_t) noexcept;
static DeleteArrayAlignFuncType real_delete_array_align = NONE;

// Unused.
// using NewPlacementFuncType                                = void* (*) (std::size_t, void* ptr) noexcept;
// static NewPlacementFuncType real_new_placement            = NONE;
// using NewArrayPlacementFuncType                           = void* (*) (std::size_t, void* ptr) noexcept;
// static NewArrayPlacementFuncType real_new_array_placement = NONE;
//
// using DeletePlacementFuncType                                   = void (*)(void*, void*) noexcept;
// static DeletePlacementFuncType real_delete_placement            = NONE;
// using DeleteArrayPlacementFuncType                              = void (*)(void*, void*) noexcept;
// static DeleteArrayPlacementFuncType real_delete_array_placement = NONE;

#endif
#endif

static int log_fd = -1;
#define WRITE_TIME(array, time)                                                                                        \
    (array)[0] = ((time).tv_sec >> 24) & 0xFF;                                                                         \
    (array)[1] = ((time).tv_sec >> 16) & 0xFF;                                                                         \
    (array)[2] = ((time).tv_sec >> 8) & 0xFF;                                                                          \
    (array)[3] = (time).tv_sec & 0xFF;                                                                                 \
    (array)[4] = ((time).tv_usec >> 24) & 0xFF;                                                                        \
    (array)[5] = ((time).tv_usec >> 16) & 0xFF;                                                                        \
    (array)[6] = ((time).tv_usec >> 8) & 0xFF;                                                                         \
    (array)[7] = (time).tv_usec & 0xFF;

#define WRITE_TIME_DIFFERENCE(array)                                                                                   \
    struct timeval current_time {};                                                                                    \
    gettimeofday(&current_time, NONE);                                                                                 \
    WRITE_TIME(array, current_time)

#define LOG_MEMORY(key, args)                                                                                          \
    unsigned char length = (unsigned char) sizeof(args) / sizeof(uint64_t);                                           \
    unsigned char array[10 + (sizeof(uint64_t) * length)];                                                            \
    WRITE_TIME_DIFFERENCE(array)                                                                                       \
    array[8] = key;                                                                                                    \
    array[9] = length;                                                                                                 \
    for (unsigned char i = 0; i < length; i++) {                                                                       \
        for (int j = 0; j < 8; j++) {                                                                                  \
            uint64_t value = (args)[i];                                                                               \
            array[(i * 8) + 10 + j] = (value >> (8 * (7 - j))) & 0xFF;                                                 \
        }                                                                                                              \
    }                                                                                                                  \
    size_t written = write(log_fd, array, 10 + (sizeof(uint64_t) * length));                                          \
    if (written < 1) {                                                                                                 \
        fprintf(stderr, ERROR_PREFIX "Error writing to log file\n");                                                   \
    }

#ifdef MEM_TEST
// stderr is loaded before everything
#define DEBUG(str, ...) fprintf(stderr, str, ##__VA_ARGS__);
#else
#define DEBUG(...)
#endif

void __attribute__((constructor)) lib_init() {
    char FILE_NAME[] = FILE_PATH;
    int i = 1;
    while (access(FILE_NAME, F_OK) == 0) {
        FILE_NAME[FILE_INDEX] = (char) (i + '0');
        i++;
    }

    log_fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (log_fd < 0) {
        fprintf(stderr, ERROR_PREFIX "Failed to open file %s\n", FILE_NAME);
    }

#ifdef __cplusplus
    real_new = (NewFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW);
    real_new_nothrow = (NewFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_NEW_NOTHROW);
    real_new_array = (NewArrayFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY);
    real_new_array_nothrow = (NewArrayFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY_NOTHROW);

    real_delete = (DeleteFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE);
    real_delete_sized = (DeleteFuncSizedType) dlsym(RTLD_NEXT, SYMBOL_DELETE_SIZED);
    real_delete_nothrow = (DeleteFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_DELETE_NOTHROW);
    real_delete_array = (DeleteArrayFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ARRAY);
    real_delete_array_sized = (DeleteArrayFuncSizedType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ARRAY_SIZED);
    real_delete_array_nothrow = (DeleteArrayFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY_NOTHROW);

#if __cplusplus >= CPP_17
    real_new_align = (NewAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW_ALIGN);
    real_new_array_align = (NewArrayAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY_ALIGN);
    real_delete_align = (DeleteAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ALIGN);
    real_delete_array_align = (DeleteArrayAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ARRAY_ALIGN);
#endif

#endif
}

void __attribute__((destructor)) lib_cleanup() {
    close(log_fd);
}

#ifdef __cplusplus
extern "C" {
#endif

void* malloc(size_t size) {
    LOAD_SYMBOL(malloc, void* (*) (size_t))
    void* ptr = real_malloc(size);
    DEBUG(MALLOC_FORMAT_STR, size, ptr);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(MALLOC, args);
    return ptr;
}

void* calloc(size_t nmemb, size_t size) {
    LOAD_SYMBOL(calloc, void* (*) (size_t, size_t))
    void* ptr = real_calloc(nmemb, size);
    DEBUG(CALLOC_FORMAT_STR, nmemb, size, ptr);
    uint64_t args[3] = {nmemb, size, (uint64_t) ptr};
    LOG_MEMORY(CALLOC, args);
    return ptr;
}

void* reallocarray(void* ptr, size_t nmemb, size_t size) {
    LOAD_SYMBOL(reallocarray, void* (*) (void*, size_t, size_t))
    void* new_ptr = real_reallocarray(ptr, nmemb, size);
    DEBUG(REALLOC_ARRAY_FORMAT_STR, ptr, nmemb, size, new_ptr);
    uint64_t args[3] = {(uint64_t) ptr, nmemb, size};
    LOG_MEMORY(REALLOC_ARRAY, args);
    return new_ptr;
}

void* realloc(void* ptr, size_t size) {
    LOAD_SYMBOL(realloc, void* (*) (void*, size_t))
    void* new_ptr = real_realloc(ptr, size);
    DEBUG(REALLOC_FORMAT_STR, ptr, size, new_ptr);
    uint64_t args[3] = {(uint64_t) ptr, size, (uint64_t) new_ptr};
    LOG_MEMORY(REALLOC, args);
    return new_ptr;
}

void free(void* ptr) {
    LOAD_SYMBOL(free, void (*)(void*))
    DEBUG(FREE_FORMAT_STR, ptr);
    real_free(ptr);
    uint64_t args[1] = {(uint64_t) ptr};
    LOG_MEMORY(FREE, args);
}

#ifdef __cplusplus
}

void* operator new(std::size_t size) {
    void* ptr = real_new(size);
    DEBUG(NEW_FORMAT_STR, size, ptr);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(NEW, args);
    return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow) {
    void* ptr = real_new_nothrow(size, nothrow);
    DEBUG(NEW_NOTHROW_FORMAT_STR, size, ptr);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(NEW_NOTHROW, args);
    return ptr;
}

void* operator new[](size_t size) {
    void* ptr = real_new_array(size);
    DEBUG(NEW_ARRAY_FORMAT_STR, size, ptr);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(NEW_ARRAY, args);
    return ptr;
}

void* operator new[](size_t size, const std::nothrow_t& nothrow) noexcept {
    void* ptr = real_new_array_nothrow(size, nothrow);
    DEBUG(NEW_ARRAY_NOTHROW_FORMAT_STR, size, ptr);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(NEW_ARRAY_NOTHROW, args);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    real_delete(ptr);
    DEBUG(DELETE_FORMAT_STR, ptr);
    uint64_t args[1] = {(uint64_t) ptr};
    LOG_MEMORY(DELETE, args);
}

void operator delete(void* ptr, std::size_t size) {
    real_delete_sized(ptr, size);
    DEBUG(DELETE_SIZED_FORMAT_STR, ptr, size);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(DELETE_SIZED, args);
}

void operator delete(void* ptr, const std::nothrow_t& nothrow) noexcept {
    real_delete_nothrow(ptr, nothrow);
    DEBUG(DELETE_NOTHROW_FORMAT_STR, ptr)
    uint64_t args[1] = {(uint64_t) ptr};
    LOG_MEMORY(DELETE_NOTHROW, args);
}

void operator delete[](void* ptr) {
    real_delete_array(ptr);
    DEBUG(DELETE_ARRAY_FORMAT_STR, ptr)
    uint64_t args[1] = {(uint64_t) ptr};
    LOG_MEMORY(DELETE_ARRAY, args);
}

void operator delete[](void* ptr, std::size_t size) {
    real_delete_array_sized(ptr, size);
    DEBUG(DELETE_ARRAY_SIZED_FORMAT_STR, ptr, size);
    uint64_t args[2] = {(uint64_t) ptr, size};
    LOG_MEMORY(DELETE_ARRAY_SIZED, args);
}

void operator delete[](void* ptr, const std::nothrow_t& nothrow) noexcept {
    real_delete_array_nothrow(ptr, nothrow);
    DEBUG(DELETE_ARRAY_NOTHROW_FORMAT_STR, ptr);
    uint64_t args[1] = {(uint64_t) ptr};
    LOG_MEMORY(DELETE_ARRAY_NOTHROW, args);
}

#if __cplusplus >= CPP_17

void* operator new(std::size_t size, std::align_val_t align) {
    void* ptr = real_new_align(size, align);
    DEBUG(NEW_ALIGN_FORMAT_STR, size, CONVERT_ALIGN(align), ptr);
    uint64_t args[3] = {(uint64_t) ptr, size, CONVERT_ALIGN(align)};
    LOG_MEMORY(NEW_ALIGN, args);
    return ptr;
}

void* operator new[](std::size_t size, std::align_val_t align) {
    void* ptr = real_new_array_align(size, align);
    DEBUG(NEW_ARRAY_ALIGN_FORMAT_STR, size, CONVERT_ALIGN(align), ptr);
    uint64_t args[3] = {(uint64_t) ptr, size, CONVERT_ALIGN(align)};
    LOG_MEMORY(NEW_ARRAY_ALIGN, args);
    return ptr;
}

void operator delete(void* ptr, std::align_val_t align) {
    real_delete_align(ptr, align);
    DEBUG(DELETE_ALIGN_FORMAT_STR, ptr, CONVERT_ALIGN(align));
    uint64_t args[2] = {(uint64_t) ptr, CONVERT_ALIGN(align)};
    LOG_MEMORY(DELETE_ALIGN, args);
}

void operator delete[](void* ptr, std::align_val_t align) {
    real_delete_array_align(ptr, align);
    DEBUG(DELETE_ARRAY_ALIGN_FORMAT_STR, ptr, CONVERT_ALIGN(align));
    uint64_t args[2] = {(uint64_t) ptr, CONVERT_ALIGN(align)};
    LOG_MEMORY(DELETE_ARRAY, args);
}


#endif // __cplusplus >= CPP_17
#endif // __cplusplus
