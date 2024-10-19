#ifndef MEM_THREADING_H
#define MEM_THREADING_H

#include <stddef.h>
#include <pthread.h>


struct mem_writer_value {
    struct mem_writer_value *next;

    char* data;
};

struct mem_writer_queue {
    struct mem_writer_value *head;
    struct mem_writer_value *tail;

    unsigned int size;

    pthread_mutex_t _head_lock;
    pthread_mutex_t _tail_lock;
    pthread_mutex_t _size_lock;
    pthread_cond_t _head_cond;
    pthread_cond_t _size_cond;
};



void mem_writer_queue_init(struct mem_writer_queue *queue);

void mem_writer_queue_destroy(struct mem_writer_queue *queue);

void add_to_mem_writer_queue(struct mem_writer_queue *queue, char* data);

void* pop_from_mem_writer_queue(struct mem_writer_queue *queue);


#endif //MEM_THREADING_H
