/**
 * @brief Common Queue functions and structs used by both single and double-ended queues
 *
 * @file common_queue.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-17
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

// Forward declaration
typedef struct queue_boundary queue_boundary;

// Properties of all queues
typedef struct
{
    uint8_t *buffer_base;
    size_t element_size;
    size_t num_free_elements;          // needed to handle adding first element
    pthread_mutex_t data_protection_m; // Needed for negotiating read/write access for num_free_elements
    uintptr_t buffer_mask;             // = size-1;
    queue_boundary *head_boundary; // Required for handling first element
} common_queue_attributes;

// Boundary markers within queues to coordinate read/write access
struct queue_boundary
{
    uint8_t *active_element;           // Active element. May be NULL if no longer active)
    uint8_t *next_element;             // Next element to activate. Always points to a valid address
    pthread_cond_t *blocking_cv;       // If CV not null, will block advancing to next until signaled by next_boundary.
    pthread_mutex_t *blocking_m;       // Associated mutex for above CV
    queue_boundary *previous_boundary; // What this boundary gates
    queue_boundary *next_boundary;     // What this boundary is gated by
    common_queue_attributes *attr;     // Pointer to commom queue attributes for advancement calculations
};

// Queue function return types
typedef enum {
    QUEUE_SUCCESS,
    QUEUE_FAILURE,
} queue_status;

/**
 * @brief Rounds up to nearest power of 2
 *
 * @param n input value
 * @return size_t input value rounded up to next power of 2
 */
size_t pow2_roundup(size_t n);

/**
 * @brief Initializes condition variable and associated mutex
 *
 * @param c double pointer to condition variable
 * @param m double pointer to mutex
 */
void init_cv_and_mutex(pthread_cond_t **c, pthread_mutex_t **m);
/**
 * @brief Deallocates condition variable and associated mutex
 *
 * @param c double pointer to condition variable
 * @param m double pointer to mutex
 */
void destroy_cv_and_mutex(pthread_cond_t **c, pthread_mutex_t **m);

/**
 * @brief Initializes common attributes of queues.
 * Alligns queue memory region with total size of queue.
 * Rounds-up both element size and number of elements to next power of 2
 * when calculating size. This makes calculations involved in navigating
 * queue easier.
 *
 * @param attr pointer to common queue attributes
 * @param element_size size of elements in queue
 * @param total_elements total number of elements in queue
 * @return queue_status Success or failure
 */
queue_status common_queue_init(common_queue_attributes *attr, size_t element_size, size_t total_elements);

/**
 * @brief Frees queue memory and resets other queue attributes
 *
 * @param attr pointer to queue attributes
 * @return queue_status success or failure
 */
queue_status common_queue_destroy(common_queue_attributes *attr);

/**
 * @brief Initializes a queue boundary.
 * These boundaries have signaling mechanisms to allow blocking between threads
 * when queue is full or empty.
 *
 * @param b pointer to bound that needs to be initialized
 * @param next_b pointer to the next boundary that this boundary is gated by
 * @param attr pointer to common queue attributes needed by boundary operations
 * @param blocked_by_next whether attempting to advance this boundary blocks if gated by next,
 * or just immediately returns NULL instead
 */
void init_boundary(queue_boundary *b,
                   queue_boundary *next_b,
                   common_queue_attributes *attr,
                   bool blocked_by_next);
void destroy_boundary(queue_boundary *b);

/**
 * @brief Determines address of next queue element, given current position and queue attributes.
 * Handles wrap-around.
 *
 * @param current address of current element
 * @param attr pointer to queue attributes
 * @return uint8_t* address of next element
 */
uint8_t *calculate_next_element(uint8_t *current, common_queue_attributes *attr);

/**
 * @brief Attempts to advance boundary to next element, but checks if gated by next element.
 *
 * @param b pointer to boundary we wish to advance
 * @param force_noblock flag to disable blocking (if enabled by default for this boundary) and immediately return NULL instead if gated by next boundary.
 * @return uint8_t* address of next element. NULL if gated and not blocking. Blocks if blocking enabled.
 */
uint8_t *boundary_get_next_active_element(queue_boundary *b, bool force_noblock);

/**
 * @brief Releases element held by current boundary, while still maintaining boundary position.
 * This allows boundaries to exist, while not consuming any queue elements, after the boundary owner
 * is done with reading / writing to queue element.
 *
 * @param b pointer to boundary with element to release
 */
void boundary_done_with_active_element(queue_boundary *b);
