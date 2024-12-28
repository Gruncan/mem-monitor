#ifndef MEM_THREADING_H
#define MEM_THREADING_H

#include <pthread.h>


struct mtc_value_s {
    void* data;
    unsigned int length;
};

struct mem_value_s {
    struct mem_value_s* next;

    struct mtc_value_s* value;
};

typedef struct mem_queue_s {
    struct mem_value_s* head;
    struct mem_value_s* tail;

    unsigned int size;

    pthread_mutex_t _lock;
    pthread_cond_t _lock_cond;
} MemQueue;


void mem_queue_init(MemQueue* queue);

void mem_queue_destroy(MemQueue* queue);

void add_to_mem_queue(MemQueue* queue, void* data, unsigned int length);

struct mtc_value_s* pop_from_mem_queue(MemQueue* queue);


#endif // MEM_THREADING_H
