/**
 * @brief Single-ended queue for 1-way communication
 *
 * @file single_ended_queue.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-17
 */

#pragma once

#include "common_queue.h"

/**
 * @brief Single-ended queue structure for 1-way communication
 */
typedef struct
{
    common_queue_attributes attr;
    queue_boundary write_b;
    queue_boundary read_b;
} sequeue;

/**
 * @brief Initializes single-ended queue.
 * Params element_size and total_elements are both rounded-up to nearest power
 * of 2, then multiplied for total queue size. Creates dynamic memory. Alligns
 * on power of 2.
 *
 * @param q pointer to single-ended queue structure
 * @param element_size size of largest element in queue
 * @param total_elements number of elements in queue
 * @return queue_status success or failure
 */
queue_status sequeue_init(sequeue *q, size_t element_size, size_t total_elements);

/**
 * @brief Destroys single-ended queue. Frees memory.
 * Note, this should be done last after threads depending on queue are killed.
 *
 * @param q pointer to single-ended queue structure
 * @return queue_status success or failure
 */
queue_status sequeue_destroy(sequeue *q);


/* Non Blocking Functions */

/**
 * @brief Indicates that write has completed.
 * Signals any potentially waiting readers waiting for next element in empty queue.
 *
 * @param q pointer to queue struct
 */
void sequeue_done_writing(sequeue *q);

/**
 * @brief Indicates that read has completed.
 * Signals any potentially waiting writers waiting for free element in full queue.
 *
 * @param q pointer to queue struct
 */
void sequeue_done_reading(sequeue *q);


/* Blocking Functions */

/**
 * @brief Gets address of next element for writing
 *
 * @param q pointer to queue struct
 * @param force_noblock flag to disable blocking and immediately return NULL instead
 * @return uint8_t* address of next empty element. NULL if gated and not blocking. Blocks if blocking enabled.
 */
uint8_t *sequeue_next_empty(sequeue *q, bool force_noblock);

/**
 * @brief Gets address of next element for reading
 *
 * @param q pointer to queue struct
 * @param force_noblock flag to disable blocking and immediately return NULL instead
 * @return uint8_t* address of next written element to read. NULL if gated and not blocking. Blocks if blocking enabled.
 */
uint8_t *sequeue_read_next(sequeue *q, bool force_noblock);
