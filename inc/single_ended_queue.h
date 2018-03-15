#pragma once

#include "common_queue.h"

// Single-ended queue
typedef struct
{
    common_queue_attributes attr;
    queue_boundary write_b;
    queue_boundary read_b;
} sequeue;

queue_status sequeue_init(sequeue *q, size_t element_size, size_t total_elements);

queue_status sequeue_destroy(sequeue *q);

/* Not Blocking */
void sequeue_done_writing(sequeue *q);

void sequeue_done_reading(sequeue *q);

/* Blocking */
uint8_t *sequeue_next_empty(sequeue *q);

uint8_t *sequeue_read_next(sequeue *q);
