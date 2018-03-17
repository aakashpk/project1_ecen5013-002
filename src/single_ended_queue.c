#include "single_ended_queue.h"

queue_status sequeue_init(sequeue *q, size_t element_size, size_t total_elements)
{
    if (common_queue_init(&q->attr, element_size, total_elements) == QUEUE_FAILURE)
    {
        return QUEUE_FAILURE;
    }

    /*
        In order of buffer addresses (ignoring wrap around)
        low > high
        read_response > write_request
        prev > next
        Actual initial boundary progression happens in reverse order,
        but we keep the above prev/next naming scheme.
    */
    init_boundary(
        &q->read_b,
        &q->write_b,
        &q->attr,
        true); // Read blocked by write

    init_boundary( // HEAD boundary
        &q->write_b,
        &q->read_b,
        &q->attr,
        true); // Write blocked by read

    q->attr.head_boundary = &q->write_b;

    if (pthread_mutex_init(&q->attr.data_protection_m, NULL))
    {
        //perror("issue with mutex init");
        abort();
    }

    return QUEUE_SUCCESS;
}

queue_status sequeue_destroy(sequeue *q)
{
    if (common_queue_destroy(&q->attr) == QUEUE_FAILURE)
    {
        return QUEUE_FAILURE;
    }

    destroy_boundary(&q->read_b);
    destroy_boundary(&q->write_b);

    return QUEUE_SUCCESS;
}

/* Not Blocking */
void sequeue_done_writing(sequeue *q)
{
    if (!q) abort();

    boundary_done_with_active_element(&q->write_b);
}

void sequeue_done_reading(sequeue *q)
{
    if (!q) abort();

    boundary_done_with_active_element(&q->read_b);
}

/* Blocking */
uint8_t *sequeue_next_empty(sequeue *q, bool force_noblock)
{
    if (!q) abort();

    return boundary_get_next_active_element(&q->write_b, force_noblock);
}

uint8_t *sequeue_read_next(sequeue *q, bool force_noblock)
{
    if (!q) abort();

    return boundary_get_next_active_element(&q->read_b, force_noblock);
}