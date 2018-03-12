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

typedef struct
{
    uint8_t* buffer_base;
    size_t element_size;
    //size_t total_elements; // probably unnecessary
    size_t element_count; // needed to handle adding first element
    uintptr_t buffer_mask; // = size-1;
    //uintptr_t buffer_inv_mask; // = ~buffer_mask;

} common_queue_attributes;

typedef struct queue_boundary queue_boundary;
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

void init_boundary(queue_boundary *b,
                   queue_boundary *next_b,
                   common_queue_attributes *attr,
                   bool blocked_by_next)
{
    b->attr = attr;
    b->active_element = NULL;
    b->next_element = attr->buffer_base;
    // May initialize these below
    b->blocking_cv = NULL;
    b->blocking_m = NULL;

    if (next_b) // Head does not have a next entry
    {
        b->next_boundary = next_b;
        // Configures next's previous pointer
        b->next_boundary->previous_boundary = b;

        if (blocked_by_next)
        {
            b->blocking_cv = malloc(sizeof(pthread_cond_t));
            b->blocking_m = malloc(sizeof(pthread_mutex_t));
            if (!b->blocking_cv || !b->blocking_m)
            {
                printf("error with malloc cv or mutex");
                abort();
            }
            // Todo - may want to double-check that there are no special attributes to pass
            if (pthread_cond_init(b->blocking_cv, NULL) ||
                pthread_mutex_init(b->blocking_m, NULL))
            {
                perror("issue with cv or mutex init");
                abort();
            }
        }
    }
    else if (blocked_by_next)
    {
        printf("Cannot be blocked by next if next is null\n");
        abort();
    }
}

void destroy_boundary(queue_boundary *b)
{
    // Destroy malloc-ed items if they exist
    if (b->blocking_cv)
    {
        pthread_cond_destroy(b->blocking_cv);
        free(b->blocking_cv);
        b->blocking_cv = NULL;
    }
    if (b->blocking_m)
    {
        pthread_mutex_destroy(b->blocking_m);
        free(b->blocking_m);
        b->blocking_m = NULL;
    }
    // Reset other elements to NULL
    // Not super important
    b->attr = NULL;
    b->active_element = NULL;
    b->next_element = NULL;
    b->next_boundary = NULL;
    b->previous_boundary = NULL;
}

void boundary_done_with_active_element(queue_boundary *b)
{
    if (!b) abort();
    if (!b->active_element)
    {
        printf("element already inactive\n");
        abort(); // Not a critical error
        return;
    }

    // Disable active element
    b->active_element = NULL;

    // Special handling for TAIL boundary
    if (!b->previous_boundary)
    {
        b->attr->element_count--;
        // HEAD is never blocked by TAIL
    }
    // Regular non-TAIL boundary
    else if (b->previous_boundary->blocking_cv)
    { // Signal potentially waiting thread
        if (!b->previous_boundary->blocking_m) abort();

        // Signal previous boundary
        pthread_mutex_lock(b->previous_boundary->blocking_m);
        pthread_cond_signal(b->previous_boundary->blocking_cv);
        pthread_mutex_unlock(b->previous_boundary->blocking_m);
    }
}

uint8_t *calculate_next_element(uint8_t *current, common_queue_attributes* attr)
{
    if (!current || !attr) abort();

    // Calculate next element location (accounting for wrap-around)
    return (uint8_t *)((uintptr_t)attr->buffer_base +
                       ((uintptr_t)(current + attr->element_size) & attr->buffer_mask));
}

uint8_t *boundary_get_next_active_element(queue_boundary *b)
{
    if (!b) abort();
    if (!b->next_element) abort();

    if (!b->next_boundary) // This is the special HEAD boundary
    {
        // Requires count-based handling
        // Could spare these identical multiplications by adding another attr member
        if (b->attr->element_count * b->attr->element_size > b->attr->buffer_mask)
        {
            // Always non-blocking
            return NULL;
        }
        b->attr->element_count++;
    }
    // Regular non-HEAD boundary, check for conflict using next boundary
    else if ((b->next_element == b->next_boundary->active_element) ||
             (b->next_element == b->next_boundary->next_element))
    {
        // Boundary conflict
        if (b->blocking_cv)
        {
            if (!b->blocking_m) abort();

            // Block on next boundary
            pthread_mutex_lock(b->blocking_m);
            pthread_cond_wait(b->blocking_cv, b->blocking_m);
            pthread_mutex_unlock(b->blocking_m);

            // Do another error check just for debugging purposes
            if ((b->next_element == b->next_boundary->active_element) ||
                (b->next_element == b->next_boundary->next_element))
                abort();
        }
        else
        {
            // Non-Blocking case
            return NULL;
        }
    }

    // No boundary conflict here

    // Not allowing multiple active elements.
    // Forces previous active element to advance if not NULL.
    // So signal previous boundary if active element is not NULL.
    if (b->active_element) boundary_done_with_active_element(b);
    // Some redundancies in above function, but worth it to avoid code dulpication

    // No need for data protection mutex if active element set before advancing next.
    // Previous boundary will be effectively blocked

    // Advance to next element
    b->active_element = b->next_element;

    // Set next element location (accounting for wrap-around)
    b->next_element = calculate_next_element(b->next_element, b->attr);

    return b->active_element;
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
    common_queue_attributes attr;
    queue_boundary write_request_b;
    queue_boundary read_request_b;
    queue_boundary write_response_b;
    queue_boundary read_response_b;
} bdqueue;


/*
    Requester Writing
    Responder Not Yet Wrote
    Responder Wrote
    responder



    Requester Readed
    Requester Reading


*/
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

    element_size = next_pow2(element_size);
    total_elements = next_pow2(total_elements);

    size_t total_size = element_size * total_elements;

    // Allocate total memory on alligned address
    q->attr.buffer_base = aligned_alloc(total_size, total_size);

    // Check that alloc succeeded
    if (!q->attr.buffer_base) return QUEUE_FAILURE;

    q->attr.element_size = element_size;
    q->attr.buffer_mask = (uintptr_t)(total_size - 1);

    /*
        In order of buffer addresses (ignoring wrap around)
        low > high
        read_response > write_response > read_request > write_request
        prev > next
        Actual initial boundary progression happens in reverse order,
        but we keep the above prev/next naming scheme.
    */
    init_boundary( // TAIL boundary (determined by HEAD init below)
        &q->read_response_b,
        &q->write_response_b,
        &q->attr,
        true); // Requester read_response blocked by Responder write_response

    init_boundary(
        &q->write_response_b,
        &q->read_request_b,
        &q->attr,
        false); // Responder write_response NOT blocked by Responder read_request

    init_boundary(
        &q->read_request_b,
        &q->write_request_b,
        &q->attr,
        true); // Responder read_request blocked by Requester write_request

    init_boundary(
        &q->write_request_b,
        NULL, // HEAD boundary
        &q->attr,
        false); // Requester write_request NOT blocked by Requester read_response

    return QUEUE_SUCCESS;
}

/*
   note, this should be done last after threads depending on queue are killed.
   frees memory
   */
queue_status bdqueue_destroy(bdqueue* q)
{
    // Not a critical check, but likely points to an error elsewhere
    if (!q->attr.buffer_base) return QUEUE_FAILURE;

    free(q->attr.buffer_base);
    q->attr.buffer_base = NULL;

    q->attr.buffer_mask = 0;
    q->attr.element_size = 0;

    destroy_boundary(&q->read_response_b);
    destroy_boundary(&q->write_response_b);
    destroy_boundary(&q->read_request_b);
    destroy_boundary(&q->write_request_b);

    return QUEUE_SUCCESS;
}

/*
   Called by requester.
   Provides pointer to next available slot for writing request.
   Does not block, since requester is responsible for reading response.
   */
uint8_t* bdqueue_next_empty_request(bdqueue* q)
{
    if (!q) abort();

    return boundary_get_next_active_element(&q->write_request_b);
}

/*
    Called by requester
*/
void bdqueue_done_populating_request(bdqueue* q)
{
    if (!q) abort();

    boundary_done_with_active_element(&q->write_request_b);
}

/*
   Called by responder
   */
void bdqueue_done_reading_request(bdqueue* q)
{
    if (!q) abort();

    boundary_done_with_active_element(&q->read_request_b);
}

/*
   Called by responder
   */
void bdqueue_done_populating_response(bdqueue* q)
{
    if (!q) abort();

    boundary_done_with_active_element(&q->write_response_b);
}

/*
   Called by requester
   */
void bdqueue_done_reading_response(bdqueue* q)
{
    if (!q) abort();

    boundary_done_with_active_element(&q->read_response_b);
}

// Blocking functions

/*
   Called by responder
   Provides pointer to next available request for responder to read
   */
uint8_t* bdqueue_next_request(bdqueue* q)
{
    if (!q) abort();

    return boundary_get_next_active_element(&q->read_request_b);
}

/*
   Called by requester
   Provides pointer to next available response for requester to read
   */
uint8_t* bdqueue_next_response(bdqueue* q, bool blocking)
{
    if (!q) abort();

    return boundary_get_next_active_element(&q->read_response_b);
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

    printf("head %p\n", myq->attr.buffer_base);
    for (int i = 0; i < 10; i++)
    {
        printf("%d %p\n", i, bdqueue_next_empty_request(myq));
    }

    return 0;
}
