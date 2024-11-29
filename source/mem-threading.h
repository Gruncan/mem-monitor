#ifndef MEM_THREADING_H
#define MEM_THREADING_H

#include <pthread.h>


struct mtc_value {
    void* data;
    unsigned int length;
};

struct mem_value {
    struct mem_value* next;

    struct mtc_value* value;
};

struct mem_queue {
    struct mem_value* head;
    struct mem_value* tail;

    unsigned int size;

    pthread_mutex_t _head_lock;
    pthread_mutex_t _tail_lock;
    pthread_mutex_t _size_lock;
    pthread_cond_t _head_cond;
    pthread_cond_t _size_cond;
};



void mem_queue_init(struct mem_queue *queue);

void mem_queue_destroy(struct mem_queue *queue);

void add_to_mem_queue(struct mem_queue *queue, void* data, unsigned int length);

struct mtc_value* pop_from_mem_queue(struct mem_queue *queue);


#endif //MEM_THREADING_H
