/**
 * @brief Bidirectional circular buffer for each requester/responder pair to
 * support 2-way request/response communication (bdqueue). Requires pair of
 * read/write pointers for requester (main) and responder (task). This enables
 * a 2x large queue for the same memory to reduce the risk of the queue
 * completely filling. This also allows responder to just fill in some struct
 * fields without needing to copy back overhead into a separate response queue.
 *
 * @file bidirectional_queue.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-17
 */
#pragma once

#include "common_queue.h"

/**
 * @brief Bidirectional queue structure for 2-way communication
 */
typedef struct
{
    common_queue_attributes attr;
    queue_boundary write_request_b;
    queue_boundary read_request_write_response_b;
    queue_boundary read_response_b;
} bdqueue;

// Non-Blocking functions

/**
 * @brief Initializes bidirectional queue.
 * Params element_size and total_elements are both rounded-up to nearest power
 * of 2, then multiplied for total queue size. Creates dynamic memory. Alligns
 * on power of 2.
 *
 * @param q pointer to bidirectional queue structure
 * @param element_size size of largest element in queue
 * @param total_elements number of elements in queue
 * @return queue_status success or failure
 */
queue_status bdqueue_init(bdqueue *q, size_t element_size, size_t total_elements);

/**
 * @brief Destroys bidirectional queue. Frees memory.
 * Note, this should be done last after threads depending on queue are killed.
 *
 * @param q pointer to bidirectional queue structure
 * @return queue_status success or failure
 */
queue_status bdqueue_destroy(bdqueue *q);

/**
 * @brief Provides pointer to next available slot for writing request.
 * Called by requester.
 * Does not block, since requester is responsible for reading response.
 *
 * @param q pointer to queue struct
 * @return uint8_t* pointer to next empty element.
 * If no empty request slots remain, returns NULL or blocks if blocking.
 */
uint8_t *bdqueue_next_empty_request(bdqueue *q);

/**
 * @brief Marks request as being completely written, so it can be read by responder.
 * Called by requester.
 *
 * @param q pointer to queue struct
 */
void bdqueue_done_writing_request(bdqueue *q);

/**
 * @brief Marks request as being read and response written, so it can be read by requester.
 * Called by responder.
 *
 * @param q pointer to queue struct
 */
void bdqueue_done_reading_request_and_writing_response(bdqueue *q);

/**
 * @brief Marks response as being read, so it can be converted to a free spot for another request.
 * Called by requester.
 *
 * @param q pointer to queue struct
 */
void bdqueue_done_reading_response(bdqueue *q);


// Potentially blocking functions

/**
 * @brief Provides pointer to next available request for responder to read.
 * Called by responder
 *
 * @param q pointer to queue struct
 * @param force_noblock flag to disable blocking and immediately return NULL instead
 * @return uint8_t* address of next request. NULL if gated and not blocking. Blocks if blocking enabled.
 */
uint8_t *bdqueue_next_request(bdqueue *q, bool force_noblock);

/**
 * @brief Provides pointer to next available response for requester to read
 * Called by requester
 *
 * @param q pointer to queue struct
 * @param force_noblock flag to disable blocking and immediately return NULL instead
 * @return uint8_t* address of next response. NULL if gated and not blocking. Blocks if blocking enabled.
 */
uint8_t *bdqueue_next_response(bdqueue *q, bool force_noblock);