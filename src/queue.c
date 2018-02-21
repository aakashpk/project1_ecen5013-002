/**
 * @file queue.c
 * @brief Support for traditional queue and bidirectional queue
 * @author Miles Frain
 * @author Aakash Kumar
 * @version 1
 * @date 2018-02-20
 */

// Todo - copy some of this stuff to header

/**
 * @brief Single bidirectional circular buffer for each requester/responder
 * pair to support 2-way request/response communication (bdqueue). Requires
 * pair of read/write pointers for requester (main) and responder (task). This
 * enables a 2x large queue for the same memory to reduce the risk of the queue
 * completely filling. This also allows responder to just fill in some struct
 * fields without needing to copy back overhead into a separate response queue.
 */
typedef struct bdqueue
{
    uint8_t* buffer_head; // not required if can allocate on power-of-2 boundary.
    uint8_t* requester_write_pointer;
    uint8_t* responder_read_pointer;
    uint8_t* responder_write_pointer;
    uint8_t* requester_read_pointer;
    pthread_mutex_t requester_wrote_m;
    pthread_mutex_t responder_wrote_m;
    pthread_cond_t requester_wrote_cv;
    pthread_cond_t responder_wrote_cv;
    size_t element_size;
    size_t num_elements;
    uint64_t buffer_mask; // = size-1;
    uint64_t buffer_inv_mask; // = ~buffer_mask;
};

enum queue_status
{
    QUEUE_SUCCESS,
    QUEUE_FAILURE,
}

// Non-Blocking functions

/*
note, wondering if element_size and num_elements should be passed through pointer, or as separate arguments. Could be dangerous and confusing for users if relying on filling-out these struct fields ahead of time.
element_size and num_elements are both rounded-up to nearest power of 2, then multiplied for total queue size.
creates dynamic memory
Alligns on power of 2. Don't need to allocate double space for allignment, can use posix_memallign() or alligned_alloc(). Still wondering if bitwise operations on pointers are allowed though. https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library
initializes pointers and mask
returns error status
*/
queue_status initialize_bdqueue(bdqueue* my_bdqueue)
{
    return QUEUE_SUCCESS;
}

/*
note, this should be done last after threads depending on queue are killed.
frees memory
*/
queue_status destroy_bdqueue(bdqueue* my_bdqueue)
{
    return QUEUE_SUCCESS;
}

// Blocking functions.

//Could perhaps provide a non_blocking version of all of these which return a null pointer

/*
Called by requester
Provides pointer to next available slot for writing request
*/
uint8_t* next_empty_request(bdqueue* my_bdqueue)
{
    return my_bdqueue->buffer_head;
}

/*
Called by requester
Advances request write pointer to next available slot
*/
void done_populating_request(bdqueue* my_bdqueue)
{
}

/*
Called by responder
Provides pointer to next available slot for reading request
*/
uint8_t* next_request(bdqueue* my_bdqueue)
{
    return my_bdqueue->buffer_head;
}

/*
Called by responder
Advances response pointer to next available slot
*/
void done_populating_response(bdqueue* my_bdqueue)
{
}

/*
Called by requester
Provides pointer to next available response
*/
uint8_t* next_response(bdqueue* my_bdqueue)
{
    return my_bdqueue->buffer_head;
}

/*
Called by requester
Frees item by advancing response read pointer to next available slot
*/
void done_reading_response(bdqueue* my_bdqueue)
{
}


