#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct queue_boundary queue_boundary;

typedef struct
{
    uint8_t* buffer_base;
    size_t element_size;
    size_t num_free_elements; // needed to handle adding first element
    uintptr_t buffer_mask; // = size-1;
    queue_boundary *head_boundary; // Required for handling first element
} common_queue_attributes;

struct queue_boundary
{
    uint8_t *active_element; // Active element. May be NULL if no longer active)
    uint8_t *next_element; // Next element to activate. Always points to a valid address
    pthread_cond_t *blocking_cv; // If CV not null, will block advancing to next until signaled by next_boundary.
    pthread_mutex_t *blocking_m; // Associated mutex for above CV
    queue_boundary *previous_boundary; // What this boundary gates
    queue_boundary *next_boundary; // What this boundary is gated by
    common_queue_attributes *attr; // Pointer to commom queue attributes for advancement calculations
};

typedef enum
{
    QUEUE_SUCCESS,
    QUEUE_FAILURE,
} queue_status;

size_t pow2_roundup(size_t n);

// Double pointer not the most flexible
void init_cv_and_mutex(pthread_cond_t **c, pthread_mutex_t **m);
void destroy_cv_and_mutex(pthread_cond_t **c, pthread_mutex_t **m);

queue_status common_queue_init(common_queue_attributes *attr, size_t element_size, size_t total_elements);
queue_status common_queue_destroy(common_queue_attributes *attr);

void init_boundary(queue_boundary *b,
                   queue_boundary *next_b,
                   common_queue_attributes *attr,
                   bool blocked_by_next);
void destroy_boundary(queue_boundary *b);

uint8_t *calculate_next_element(uint8_t *current, common_queue_attributes* attr);

uint8_t *boundary_get_next_active_element(queue_boundary *b, bool force_noblock);
void boundary_done_with_active_element(queue_boundary *b);
