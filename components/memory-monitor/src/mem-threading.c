

#include "mem-threading.h"

#include <stdio.h>
#include <stdlib.h>


void mem_queue_init(struct mem_queue* queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->_lock, NULL);
    pthread_cond_init(&queue->_lock_cond, NULL);
}

void mem_queue_destroy(struct mem_queue* queue) {

    pthread_mutex_lock(&queue->_lock);
    while (queue->size > 0) {
        pthread_cond_wait(&queue->_lock_cond, &queue->_lock);
    }

    pthread_mutex_unlock(&queue->_lock);
    pthread_mutex_destroy(&queue->_lock);
    pthread_cond_destroy(&queue->_lock_cond);

}

void add_to_mem_queue(struct mem_queue *queue, void* data, unsigned int length) {
    if (queue == NULL) {
        return;
    }

    struct mtc_value* mtcValue = malloc(sizeof(struct mtc_value));
    if (mtcValue == NULL) {
        perror("Failed to allocate memory for mtc value");
        return;
    }

    struct mem_value* writer_value = malloc(sizeof(struct mem_value));
    if (writer_value == NULL) {
        perror("Failed to allocate memory for writer value");
        return;
    }

    writer_value->next = NULL;
    mtcValue->data = data;
    mtcValue->length = length;
    writer_value->value = mtcValue;

    pthread_mutex_lock(&queue->_lock);

    if(queue->tail == NULL) {
        queue->tail = writer_value;
        queue->head = writer_value;

    }else {
        queue->tail->next = writer_value;
        queue->tail = writer_value;
    }


    queue->size++;

    pthread_cond_signal(&queue->_lock_cond);
    pthread_mutex_unlock(&queue->_lock);

}

struct mtc_value* pop_from_mem_queue(struct mem_queue* queue) {
    if (queue == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&queue->_lock);

    while (queue->head == NULL) {
        pthread_cond_wait(&queue->_lock_cond, &queue->_lock);
    }

    struct mem_value* head = queue->head;

    struct mtc_value* data = head->value;

    // This is safe as we have ensured above that head is not NULL
    queue->head = queue->head->next;
    if(queue->head == NULL){
        queue->tail = NULL;
    }

    queue->size--;

    free(head);

    pthread_mutex_unlock(&queue->_lock);

    return data;
}