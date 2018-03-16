#pragma once

#include "common_queue.h"

/**
 * @brief Single bidirectional circular buffer for each requester/responder
 * pair to support 2-way request/response communication (bdqueue). Requires
 * pair of read/write pointers for requester (main) and responder (task). This
 * enables a 2x large queue for the same memory to reduce the risk of the queue
 * completely filling. This also allows responder to just fill in some struct
 * fields without needing to copy back overhead into a separate response queue.
 */
typedef struct
{
    common_queue_attributes attr;
    queue_boundary write_request_b;
    queue_boundary read_request_write_response_b;
    queue_boundary read_response_b;
} bdqueue;

// Non-Blocking functions


/*
   Wondering if element_size and total_elements should be passed through pointer,
   or as separate arguments. Could be dangerous and confusing for users if
   relying on filling-out these struct fields ahead of time.  Params
   element_size and total_elements are both rounded-up to nearest power of 2,
   then multiplied for total queue size.  Creates dynamic memory.  Alligns on
   power of 2. Don't need to allocate double space for allignment, can use
   posix_memallign() or alligned_alloc(). Still wondering if bitwise operations
   on pointers are allowed though.
   https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library
   Initializes pointers and mask Returns error status
   */
queue_status bdqueue_init(bdqueue* q, size_t element_size, size_t total_elements);



/*
   note, this should be done last after threads depending on queue are killed.
   frees memory
   */
queue_status bdqueue_destroy(bdqueue *q);


/*
   Called by requester.
   Provides pointer to next available slot for writing request.
   Does not block, since requester is responsible for reading response.
   */
uint8_t *bdqueue_next_empty_request(bdqueue *q);


/*
    Called by requester
*/
void bdqueue_done_writing_request(bdqueue* q);

/*
   Called by responder
   */
void bdqueue_done_reading_request_and_writing_response(bdqueue* q);

/*
   Called by requester
   */
void bdqueue_done_reading_response(bdqueue* q);

// Potentially blocking functions

/*
   Called by responder
   Provides pointer to next available request for responder to read
   */
uint8_t* bdqueue_next_request(bdqueue* q, bool force_noblock);

/*
   Called by requester
   Provides pointer to next available response for requester to read
   */
uint8_t* bdqueue_next_response(bdqueue* q, bool force_noblock);