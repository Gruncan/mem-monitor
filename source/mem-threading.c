

#include "mem-threading.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void mem_queue_init(struct mem_queue *queue) {
    queue->head = queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->_head_lock, NULL);
    pthread_mutex_init(&queue->_tail_lock, NULL);
    pthread_mutex_init(&queue->_size_lock, NULL);
    pthread_cond_init(&queue->_head_cond, NULL);
    pthread_cond_init(&queue->_size_cond, NULL);
}

void mem_queue_destroy(struct mem_queue *queue) {

    pthread_mutex_lock(&queue->_size_lock);
    while (queue->size > 0) {
        pthread_cond_wait(&queue->_size_cond, &queue->_size_lock);
    }
    pthread_mutex_unlock(&queue->_size_lock);

    pthread_mutex_destroy(&queue->_head_lock);
    pthread_mutex_destroy(&queue->_tail_lock);
    pthread_mutex_destroy(&queue->_size_lock);
    pthread_cond_destroy(&queue->_head_cond);
    pthread_cond_destroy(&queue->_size_cond);

}

void add_to_mem_queue(struct mem_queue *queue, char* data) {
    if (queue == NULL) {
        return;
    }

    struct mem_value* writer_value = malloc(sizeof(struct mem_value));
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

}

void* pop_from_mem_queue(struct mem_queue *queue) {
    if (queue == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&queue->_head_lock);
    struct mem_value* head = queue->head;
    while (head == NULL) {
        pthread_cond_wait(&queue->_head_cond, &queue->_head_lock);
        head = queue->head;
    }
    queue->head = head->next;


    pthread_mutex_lock(&queue->_tail_lock);
    pthread_mutex_lock(&queue->_size_lock);
    if (queue->size == 1) {
        queue->tail = NULL;
        queue->size--;
        pthread_mutex_unlock(&queue->_size_lock);
        pthread_cond_signal(&queue->_size_cond);
    }
    pthread_mutex_unlock(&queue->_tail_lock);

    pthread_mutex_unlock(&queue->_head_lock);

    char* data = head->data;
    free(head);

    return data;
}