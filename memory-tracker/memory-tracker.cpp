/*********************************
 *
 *
 *
 *********************************/
#define _GNU_SOURCE

#ifdef __cplusplus
#include <cstdio>
#include <cstring>
#include <string>
#include <new>
#include <type_traits>

#define NONE nullptr

#else
#include <stdio.h>
#include <string.h>

#define NONE NULL

#endif

#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>



#define SYMBOL_NEW                  "_Znwm"
#define SYMBOL_NEW_NOTHROW          "_ZnwmRKSt9nothrow_t"
#define SYMBOL_DELETE               "_ZdlPv"
#define SYMBOL_DELETE_SIZED         "_ZdlPvm"
#define SYMBOL_DELETE_NOTHROW       "_ZdlPvRKSt9nothrow_t"
#define SYMBOL_NEW_ARRAY            "_Znam"
#define SYMBOL_NEW_ARRAY_NOTHROW    "_ZnamRKSt9nothrow_t"
#define SYMBOL_DELETE_ARRAY         "_ZdaPv"
#define SYMBOL_DELETE_ARRAY_SIZED   "_ZdaPvm"
#define SYMBOL_DELETE_ARRAY_NOTHROW "_ZdaPvRKSt9nothrow_t"
// C++ 17 specifics
#define SYMBOL_NEW_ALIGN              "_ZnwmSt11align_val_t"
#define SYMBOL_NEW_ARRAY_ALIGN        "_ZnamSt11align_val_t"
#define SYMBOL_DELETE_ALIGN           "_ZdlPvSt11align_val_t"
#define SYMBOL_DELETE_ARRAY_ALIGN     "_ZdaPvSt11align_val_t"
#define SYMBOL_NEW_PLACEMENT          "_ZnwmPv" // Unused
#define SYMBOL_NEW_ARRAY_PLACEMENT    "_ZnamPv" // Unused
#define SYMBOL_DELETE_PLACEMENT       "_ZdlPvS_" // Unused
#define SYMBOL_DELETE_ARRAY_PLACEMENT "_ZdaPvS_" // Unused


#define CPP_17 201703L


#define FILE_NAME "memory_tracker.tmtc"
#define LOG_SIZE 50

#define LOAD_SYMBOL(symbol, funcType) \
    if(real_##symbol == NONE) { \
        real_##symbol = (funcType) dlsym(RTLD_NEXT, #symbol);   \
    }\

#define CONVERT_ALIGN(align) static_cast<std::underlying_type_t<std::align_val_t>>(align)

static void* (*real_malloc) (size_t size) = NONE;
static void* (*real_calloc) (size_t nmemb, size_t size) = NONE;
static void* (*real_realloc) (void *ptr, size_t size) = NONE;
static void* (*real_reallocarray) (void* ptr, size_t nmemb, size_t size) = NONE;
static void  (*real_free) (void *ptr) = NONE;

#ifdef __cplusplus
using NewFuncType = void* (*)(std::size_t);
static NewFuncType real_new = NONE;
using NewFuncNoThrowType = void* (*)(std::size_t, const std::nothrow_t&) noexcept;
static NewFuncNoThrowType real_new_nothrow = NONE;

using DeleteFuncType = void (*)(void*);
static DeleteFuncType real_delete = NONE;
using DeleteFuncSizedType = void (*)(void*, std::size_t);
static DeleteFuncSizedType real_delete_sized = NONE;
using DeleteFuncNoThrowType = void (*)(void*, const std::nothrow_t&) noexcept;
static DeleteFuncNoThrowType real_delete_nothrow = NONE;

using NewArrayFuncType = void* (*)(std::size_t);
static NewArrayFuncType real_new_array = NONE;
using NewArrayFuncNoThrowType = void* (*)(std::size_t, const std::nothrow_t&) noexcept;
static NewArrayFuncNoThrowType real_new_array_nothrow = NONE;

using DeleteArrayFuncType = void (*)(void*) noexcept;
static DeleteArrayFuncType real_delete_array = NONE;
using DeleteArrayFuncSizedType = void (*)(void*, std::size_t) noexcept;
static DeleteArrayFuncSizedType real_delete_array_sized = NONE;
using DeleteArrayFuncNoThrowType = void (*)(void*, const std::nothrow_t&) noexcept;
static DeleteArrayFuncNoThrowType real_delete_array_nothrow = NONE;

#if __cplusplus >= CPP_17

using NewAlignFuncType = void* (*)(std::size_t, std::align_val_t);
static NewAlignFuncType real_new_align = NONE;
using NewArrayAlignFuncType = void* (*)(std::size_t, std::align_val_t);
static NewArrayAlignFuncType real_new_array_align = NONE;

using DeleteAlignFuncType = void (*)(void*, std::align_val_t) noexcept;
static DeleteAlignFuncType real_delete_align = NONE;
using DeleteArrayAlignFuncType = void (*)(void*, std::align_val_t) noexcept;
static DeleteArrayAlignFuncType real_delete_array_align = NONE;

// using NewPlacementFuncType = void* (*)(std::size_t, void* ptr) noexcept;
// static NewPlacementFuncType real_new_placement = NONE;
// using NewArrayPlacementFuncType = void* (*)(std::size_t, void* ptr) noexcept;
// static NewArrayPlacementFuncType real_new_array_placement = NONE;
//
// using DeletePlacementFuncType = void (*)(void*, void*) noexcept;
// static DeletePlacementFuncType real_delete_placement = NONE;
// using DeleteArrayPlacementFuncType = void (*)(void*, void*) noexcept;
// static DeleteArrayPlacementFuncType real_delete_array_placement = NONE;

#endif
#endif

static int log_fd = -1;

#define LOG_MEMORY(str, ...) \
    char buffer[128];\
    int len = snprintf(buffer, sizeof(buffer)-2, str, ##__VA_ARGS__);\
    buffer[len] = '\n';  \
    buffer[len + 1] = '\0';  \
    write(log_fd, buffer, len+1);\

void __attribute__((constructor)) lib_init() {
    log_fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

#ifdef __cplusplus
    real_new               = (NewFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW);
    real_new_nothrow       = (NewFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_NEW_NOTHROW);
    real_new_array         = (NewArrayFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY);
    real_new_array_nothrow = (NewArrayFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY_NOTHROW);

    real_delete               = (DeleteFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE);
    real_delete_sized         = (DeleteFuncSizedType) dlsym(RTLD_NEXT, SYMBOL_DELETE_SIZED);
    real_delete_nothrow       = (DeleteFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_DELETE_NOTHROW);
    real_delete_array         = (DeleteArrayFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ARRAY);
    real_delete_array_sized   = (DeleteArrayFuncSizedType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ARRAY_SIZED);
    real_delete_array_nothrow = (DeleteArrayFuncNoThrowType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY_NOTHROW);

#if __cplusplus >= CPP_17
    real_new_align          = (NewAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW_ALIGN);
    real_new_array_align    = (NewArrayAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_NEW_ARRAY_ALIGN);
    real_delete_align       = (DeleteAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ALIGN);
    real_delete_array_align = (DeleteArrayAlignFuncType) dlsym(RTLD_NEXT, SYMBOL_DELETE_ARRAY_ALIGN);
#endif

#endif
    LOG_MEMORY("Memory tracker initialized.")
}

void __attribute__((destructor)) lib_cleanup() {
    LOG_MEMORY("Memory tracker cleaning up.")

    close(log_fd);
}

#ifdef __cplusplus
extern "C" {
#endif

void* malloc(size_t size) {
    LOAD_SYMBOL(malloc, void* (*)(size_t))

    void* ptr = real_malloc(size);
    LOG_MEMORY("malloc(%zd) = %p", size, ptr);
    return ptr;
}

void* calloc(size_t nmemb, size_t size) {
    LOAD_SYMBOL(calloc, void* (*)(size_t, size_t))

    void* ptr = real_calloc(nmemb, size);
    LOG_MEMORY("calloc(%zd, %zd) = %p", nmemb, size, ptr);
    return ptr;
}

void* reallocarray(void* ptr, size_t nmemb, size_t size) {
    LOAD_SYMBOL(reallocarray, void* (*)(void*, size_t, size_t))

    void* new_ptr = real_reallocarray(ptr, nmemb, size);
    LOG_MEMORY("reallocarray(%p, %zd, %zd) = %p", ptr, nmemb, size, new_ptr);
    return new_ptr;
}

void* realloc(void* ptr, size_t size) {
    LOAD_SYMBOL(realloc, void* (*)(void*, size_t))
    void* new_ptr = real_realloc(ptr, size);
    LOG_MEMORY("realloc(%p, %zd) = %p", ptr, size, new_ptr);
    return new_ptr;
}

void free(void *ptr) {
    LOAD_SYMBOL(free, void (*)(void*))
    real_free(ptr);
    LOG_MEMORY("free(%p)", ptr);
}

#ifdef __cplusplus
}

void* operator new(std::size_t size){
    void* ptr = real_new(size);
    LOG_MEMORY("new(%zd) = %p", size, ptr);
    return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow){
    void* ptr = real_new_nothrow(size, nothrow);
    LOG_MEMORY("new_nothrow(%zd) = %p", size, ptr);
    return ptr;
}

void* operator new[](size_t size) {
    void* ptr = real_new_array(size);
    LOG_MEMORY("new[](%zd) = %p", size, ptr);
    return ptr;
}

void* operator new[](size_t size, const std::nothrow_t& nothrow) noexcept {
    void* ptr = real_new_array_nothrow(size, nothrow);
    LOG_MEMORY("new_nothrow[](%zd) = %p", size, ptr);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    real_delete(ptr);
    LOG_MEMORY("delete(%p)", ptr);
}

void operator delete(void* ptr, std::size_t size){
    real_delete_sized(ptr, size);
    LOG_MEMORY("delete_sized(%p, %lu)", ptr, size);
}

void operator delete(void* ptr, const std::nothrow_t& nothrow) noexcept {
    real_delete_nothrow(ptr, nothrow);
    LOG_MEMORY("delete_nothrow(%p)", ptr)
}

void operator delete[](void* ptr){
    real_delete_array(ptr);
    LOG_MEMORY("delete[](%p)", ptr)
}

void operator delete[](void* ptr, std::size_t size){
    real_delete_array_sized(ptr, size);
    LOG_MEMORY("delete[](%p, %zd)", ptr, size);
}

void operator delete[](void* ptr, const std::nothrow_t& nothrow) noexcept {
    real_delete_array_nothrow(ptr, nothrow);
    LOG_MEMORY("delete[](%p)", ptr);
}

#if __cplusplus >= CPP_17

void* operator new(std::size_t size, std::align_val_t align){
    void* ptr = real_new_align(size, align);
    LOG_MEMORY("new_align(%zd, %zd) = %p", size, CONVERT_ALIGN(align), ptr);
    return ptr;
}

void* operator new[](std::size_t size, std::align_val_t align){
    void* ptr = real_new_array_align(size, align);
    LOG_MEMORY("new[](%zd, %zd) = %p", size, CONVERT_ALIGN(align), ptr);
    return ptr;
}

void operator delete(void* ptr, std::align_val_t align){
    real_delete_align(ptr, align);
    LOG_MEMORY("delete_align(%p, %zd)", ptr, CONVERT_ALIGN(align));
}

void operator delete[](void* ptr, std::align_val_t align){
    real_delete_array_align(ptr, align);
    LOG_MEMORY("delete_align[](%p, %zd)", ptr, CONVERT_ALIGN(align));
}


#endif // __cplusplus >= CPP_17
#endif // __cplusplus