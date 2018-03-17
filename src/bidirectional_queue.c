#include "bidirectional_queue.h"

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
queue_status bdqueue_init(bdqueue *q, size_t element_size, size_t total_elements)
{
    if (common_queue_init(&q->attr, element_size, total_elements) == QUEUE_FAILURE)
    {
        return QUEUE_FAILURE;
    }

    /*
        In order of buffer addresses (ignoring wrap around)
        low > high
        read_response > read_request_write_response > write_request
        prev > next
        Actual initial boundary progression happens in reverse order,
        but we keep the above prev/next naming scheme.
    */
    init_boundary( // TAIL boundary (determined by HEAD init below)
        &q->read_response_b,
        &q->read_request_write_response_b,
        &q->attr,
        true); // Requester read_response blocked by Responder write_response

    init_boundary(
        &q->read_request_write_response_b,
        &q->write_request_b,
        &q->attr,
        true); // Responder read_request_write_response blocked by Requester write_request

    init_boundary( // HEAD boundary
        &q->write_request_b,
        &q->read_response_b,
        &q->attr,
        false); // Requester write_request NOT blocked by Requester read_response

    q->attr.head_boundary = &q->write_request_b;

    if (pthread_mutex_init(&q->attr.data_protection_m, NULL))
    {
        //perror("issue with mutex init");
        abort();
    }

    return QUEUE_SUCCESS;
}

/*
   note, this should be done last after threads depending on queue are killed.
   frees memory
   */
queue_status bdqueue_destroy(bdqueue *q)
{
    if (common_queue_destroy(&q->attr) == QUEUE_FAILURE)
    {
        return QUEUE_FAILURE;
    }

    destroy_boundary(&q->read_response_b);
    destroy_boundary(&q->read_request_write_response_b);
    destroy_boundary(&q->write_request_b);

    return QUEUE_SUCCESS;
}

/*
   Called by requester.
   Provides pointer to next available slot for writing request.
   Does not block, since requester is responsible for reading response.
   */
uint8_t *bdqueue_next_empty_request(bdqueue *q)
{
    if (!q)
        abort();

    return boundary_get_next_active_element(&q->write_request_b, false);
}

/*
    Called by requester
*/
void bdqueue_done_writing_request(bdqueue *q)
{
    if (!q)
        abort();

    boundary_done_with_active_element(&q->write_request_b);
}

/*
   Called by responder
   */
void bdqueue_done_reading_request_and_writing_response(bdqueue *q)
{
    if (!q)
        abort();

    boundary_done_with_active_element(&q->read_request_write_response_b);
}

/*
   Called by requester
   */
void bdqueue_done_reading_response(bdqueue *q)
{
    if (!q)
        abort();

    boundary_done_with_active_element(&q->read_response_b);
}

// Potentially blocking functions

/*
   Called by responder
   Provides pointer to next available request for responder to read
   */
uint8_t *bdqueue_next_request(bdqueue *q, bool force_noblock)
{
    if (!q)
        abort();

    return boundary_get_next_active_element(&q->read_request_write_response_b, force_noblock);
}

/*
   Called by requester
   Provides pointer to next available response for requester to read
   */
uint8_t *bdqueue_next_response(bdqueue *q, bool force_noblock)
{
    if (!q)
        abort();

    return boundary_get_next_active_element(&q->read_response_b, force_noblock);
}