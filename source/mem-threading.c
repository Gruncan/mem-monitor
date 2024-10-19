

#include "mem-threading.h"

#include <stdio.h>
#include <stdlib.h>


void mem_writer_queue_init(struct mem_writer_queue *queue) {
    queue->head = queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->_head_lock, NULL);
    pthread_mutex_init(&queue->_tail_lock, NULL);
    pthread_cond_init(&queue->_head_cond, NULL);
}


void mem_writer_queue_destroy(struct mem_writer_queue *queue) {

}

void add_to_mem_writer_queue(struct mem_writer_queue *queue, char* data) {
    if (queue == NULL) {
        return;
    }

    struct mem_writer_value* writer_value = malloc(sizeof(struct mem_writer_value));
    if (writer_value == NULL) {
        perror("Failed to allocate memory for writer value");
        return;
    }

    writer_value->data = data;
    writer_value->next = NULL;

    pthread_mutex_lock(&queue->_tail_lock);

    if(queue->tail == NULL) {
        queue->tail = writer_value;

        pthread_mutex_lock(&queue->_head_lock);
        queue->head = writer_value;
        pthread_mutex_unlock(&queue->_head_lock);

        pthread_cond_signal(&queue->_head_cond);

    }else {
        queue->tail->next = writer_value;

        queue->tail = writer_value;
    }

    pthread_mutex_lock(&queue->_size_lock);
    queue->size++;
    pthread_mutex_unlock(&queue->_size_lock);

    pthread_mutex_unlock(&queue->_tail_lock);

    pthread_mutex_unlock(&queue->_head_lock);
    pthread_cond_signal(&queue->_head_cond);

}

void* pop_from_mem_writer_queue(struct mem_writer_queue *queue) {
    if (queue == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&queue->_head_lock);
    struct mem_writer_value* head = queue->head;
    while (head == NULL) {
        printf("In waiting for cond!\n");
        pthread_cond_wait(&queue->_head_cond, &queue->_head_lock);
        head = queue->head;
    }
    printf("Thread has been populated doing something..\n");
    queue->head = head->next;


    pthread_mutex_lock(&queue->_tail_lock);
    pthread_mutex_lock(&queue->_size_lock);
    if (queue->size == 1) {
        queue->tail = NULL;
        queue->size--;
    }
    pthread_mutex_unlock(&queue->_size_lock);
    pthread_mutex_unlock(&queue->_tail_lock);


    pthread_mutex_unlock(&queue->_head_lock);

    return head->data;
}