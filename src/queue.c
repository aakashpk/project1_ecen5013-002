/**
 * @file queue.c
 * @brief Support for traditional queue and bidirectional queue
 * @author Miles Frain
 * @author Aakash Kumar
 * @version 1
 * @date 2018-02-20
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

// TODO - copy some of this stuff to header


// http://locklessinc.com/articles/next_pow2/
// Only for GCC
static __attribute__((noinline)) size_t next_pow2(size_t x)
{
    // Below formula does not handle 0 and 1 correctly.
    // Also faster to handle 2 here too.
    if (x <= 2) return x;

    // TODO - ensure this works the same on 32-bit architecture.
    // May need to choose clz vs clzll.
    // Start with 0x100...000, then shift this over one less than number of
    // leading zeros of one less than input value.
    return (1UL << (sizeof(size_t) * 8 - 1)) >> (__builtin_clzll(x - 1) - 1);
}

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
    uint8_t* buffer_base; // not required if can allocate on power-of-2 boundary, although makes calculations easier.
    uint8_t* requester_write_pointer;
    uint8_t* responder_read_pointer;
    uint8_t* responder_write_pointer;
    uint8_t* requester_read_pointer;
    pthread_mutex_t requester_wrote_m;
    pthread_mutex_t responder_wrote_m;
    pthread_cond_t requester_wrote_cv;
    pthread_cond_t responder_wrote_cv;
    size_t element_size;
    size_t total_elements; // probably unnecessary
    size_t request_count; // probably unnecessary
    uintptr_t buffer_mask; // = size-1;
    uintptr_t buffer_inv_mask; // = ~buffer_mask;
} bdqueue;

typedef enum
{
    QUEUE_SUCCESS,
    QUEUE_FAILURE,
} queue_status;

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
queue_status bdqueue_init(bdqueue* q, size_t element_size, size_t total_elements)
{
    // Must have valid pointer and non-zero element size and num
    if (!q || !element_size || !total_elements) return QUEUE_FAILURE;

    q->element_size = next_pow2(element_size);
    q->total_elements = next_pow2(total_elements);

    size_t total_size = q->element_size * q->total_elements;

    // Allocate total memory on alligned address
    q->buffer_base = aligned_alloc(total_size, total_size);

    // Check that alloc succeeded
    if (!q->buffer_base) return QUEUE_FAILURE;

    // TODO set other required members
    q->requester_write_pointer = NULL;
    q->responder_read_pointer = NULL;
    q->responder_write_pointer = NULL;
    q->requester_read_pointer = NULL;

    q->request_count = 0;
    q->buffer_mask = (uintptr_t)(total_size - 1);
    q->buffer_inv_mask = ~(q->buffer_mask);

    return QUEUE_SUCCESS;
}

/*
   note, this should be done last after threads depending on queue are killed.
   frees memory
   */
queue_status bdqueue_destroy(bdqueue* q)
{
    // Not a critical check, but likely points to an error elsewhere
    if (!q->buffer_base) return QUEUE_FAILURE;

    free(q->buffer_base);

    // TODO clear other members
    q->requester_write_pointer = NULL;
    q->responder_read_pointer = NULL;
    q->responder_write_pointer = NULL;
    q->requester_read_pointer = NULL;

    q->request_count = 0;

    return QUEUE_SUCCESS;
}

/*
   Called by requester.
   Provides pointer to next available slot for writing request.
   Does not block, since requester is responsible for reading response.
   Returns NULL if called in-error.
   */
uint8_t* bdqueue_next_empty_request(bdqueue* q)
{
    if ((!q) || // Null queue
       (q->request_count >= q->total_elements)) // Queue full
    {
        printf("Error - Null queue or full of requests");
        return NULL;
    }
    // TODO - Tracking request_count seems like cleanest solution

    q->request_count++;

    // Create first request if none yet
    if (q->requester_write_pointer == NULL)
    {
        q->requester_write_pointer = q->buffer_base;
        // No need for request_count
        return q->requester_write_pointer;
    }

    // Set next write location (accounting for wrap-around)
    q->requester_write_pointer = (uint8_t*)((uintptr_t)q->buffer_base +
        ((uintptr_t)(q->requester_write_pointer + q->element_size) & q->buffer_mask));

    return q->requester_write_pointer;
}

/*
   Called by requester
   Advances request write pointer to next available slot
   */
void bdqueue_done_populating_request(bdqueue* q)
{
}

/*
   Called by responder
   Advances response write pointer to next available slot.
   Only
   */
void bdqueue_done_populating_response(bdqueue* q)
{
    if (!q) // Null queue
    {
        return;
    }

/*
    // Determine location of next request (accounting for wrap-around)
    uint8_t *next = (uint8_t*)((uintptr_t)q->buffer_base +
        ((uintptr_t)(q->responder_read_pointer + q->element_size) & q->buffer_mask));

    // Check that this location is not currently being written
    if (next == q->requester_write_pointer)
    {
        return NULL;

        // Block here
    }

    // TODO - is mutex protection (besides signals) required?

    q->responder_read_pointer = next;
    */
}

/*
   Called by requester
   Frees item by reducing number of total requests by one
   */
void bdqueue_done_reading_response(bdqueue* q)
{
    // Just clearing space for another request
    q->request_count--;
    // Requester read pointer advancement only happens in bdqueue_next_response
}

// Blocking functions

/*
   Called by responder
   Provides pointer to next available request for responder to read
   */
uint8_t* bdqueue_next_request(bdqueue* q, bool blocking)
{
    if (!q) // Null queue
    {
        return NULL;
    }

    // Determine location of next request (accounting for wrap-around)
    uint8_t *next = (uint8_t*)((uintptr_t)q->buffer_base +
        ((uintptr_t)(q->responder_read_pointer + q->element_size) & q->buffer_mask));

    // Check that this location is not currently being written
    if (next == q->requester_write_pointer)
    {
        // Just return null if not blocking
        if (!blocking)
        {
            return NULL;
        }

        // Block here
    }

    // TODO - is mutex protection (besides signals) required?

    q->responder_read_pointer = next;

    return q->responder_read_pointer;
    return q->buffer_base;
}

/*
   Called by requester
   Provides pointer to next available response for requester to read
   */
uint8_t* bdqueue_next_response(bdqueue* q, bool blocking)
{
    if (!q) // Null queue
    {
        return NULL;
    }

    // Determine location of next response (accounting for wrap-around)
    uint8_t *next = (uint8_t*)((uintptr_t)q->buffer_base +
        ((uintptr_t)(q->requester_read_pointer + q->element_size) & q->buffer_mask));

    // Check that this location is not currently being written
    if (next == q->responder_write_pointer)
    {
        // Just return null if not blocking
        if (!blocking)
        {
            return NULL;
        }

        // Block here
    }

    // TODO - is mutex protection (besides signals) required?

    q->responder_write_pointer = next;

    return q->responder_write_pointer;
}

int main()
{
    // Check valid powers of 2.
    // Should move this to separate test
    for (size_t s = 0; s < 10; s++)
    {
        printf("%zu %zu\n", s, next_pow2(s));
    }

    bdqueue *myq = malloc(sizeof(bdqueue));

    //if (bdqueue_init(myq, 1000, 1000) == QUEUE_FAILURE)
    if (bdqueue_init(myq, 3, 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }

    printf("head %p\n", myq->buffer_base);
    for (int i = 0; i < 10; i++)
    {
        printf("%d %p\n", i, bdqueue_next_empty_request(myq));
    }

    return 0;
}
