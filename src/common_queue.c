// _GNU_SOURCE must be defined before stdlib.h (from common_queue.h) for
// alligned_alloc() to be declared.
#define _GNU_SOURCE
#include "common_queue.h"

// Only for GCC
// Good article on different techniques here:
//  http://locklessinc.com/articles/next_pow2/
size_t pow2_roundup(size_t n)
{
    // Below formula does not handle 0 and 1 correctly.
    // Also faster to handle 2 here too.
    if (n <= 2)
        return n;

// TODO - ensure this works the same on 32-bit architecture.
// May need to choose clz vs clzll.
// Start with 0x100...000, then shift this over one less than number of
// leading zeros of one less than input value.
#ifdef ___X86_64__
    return (1UL << (sizeof(size_t) * 8 - 1)) >> (__builtin_clzll(n - 1) - 1);
#else
    return (1UL << (sizeof(size_t) * 8 - 1)) >> (__builtin_clzl(n - 1) - 1);
#endif
}

void init_cv_and_mutex(pthread_cond_t **c, pthread_mutex_t **m)
{
    *c = malloc(sizeof(pthread_cond_t));
    *m = malloc(sizeof(pthread_mutex_t));
    if (!*c || !*m)
    {
        //perror("error with malloc cv or mutex");
        abort();
    }
    if (pthread_cond_init(*c, NULL) ||
        pthread_mutex_init(*m, NULL))
    {
        //perror("issue with cv or mutex init");
        abort();
    }
}

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
            init_cv_and_mutex(&b->blocking_cv, &b->blocking_m);
        }
    }
    else if (blocked_by_next)
    {
        //perror("Cannot be blocked by next if next is null\n");
        abort();
    }
}

// Double pointer not the most flexible
void destroy_cv_and_mutex(pthread_cond_t **c, pthread_mutex_t **m)
{
    // Destroy malloc-ed items if they exist
    if (c && *c)
    {
        pthread_cond_destroy(*c);
        free(*c);
        *c = NULL;
    }
    if (m && *m)
    {
        pthread_mutex_destroy(*m);
        free(*m);
        *m = NULL;
    }
}

void destroy_boundary(queue_boundary *b)
{
    destroy_cv_and_mutex(&b->blocking_cv, &b->blocking_m);
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
    if (!b ||
        !b->previous_boundary || // If any of these boundaries are null, later code likely unreliable
        !b->attr ||
        !b->attr->head_boundary) // head cannot be null
    {
        abort();
    }
    if (!b->active_element)
    {
        //perror("element already inactive\n");
        abort(); // Not a critical error
        return;
    }

    // Disable active element
    b->active_element = NULL;

    // Special handling for TAIL boundary
    if (b->previous_boundary == b->attr->head_boundary) // Tail if previous is head
    {
        pthread_mutex_lock(&b->attr->data_protection_m);
        b->attr->num_free_elements++;
        pthread_mutex_unlock(&b->attr->data_protection_m);
    }

    if (b->previous_boundary->blocking_cv)
    { // Signal potentially waiting thread
        if (!b->previous_boundary->blocking_m)
            abort();

        // TODO - for greater reliability, should change condition CV depends on within mutex
        // https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032r/index.html
        /* These conditions are:
            num_free_elements
            active_element
            next_element

            Seems like there can definitely be an issue if:
                blockee checks and determines resource not available.
                --- context switch ---
                resource becomes available by blocker
                blocker signals resource available
                --- context switch ---
                blockee takes mutex
                blockee waits ... forever

                This is resolved by blockee taking mutex before checking condition,
                but this also requires blocker to take mutex before signaling,
                otherwise signal could still get lost between condition check and wait.

                blockee takes mutex
                blockee checks and determines resource not available.
                --- context switch ---
                resource becomes available by blocker
                blocker attempts to take mutex in order to signal resource available, but can't yet and must wait
                --- context switch ---
                blockee waits and gives up mutex while waiting
                --- context switch ---
                blocker finally gets mutex in order signal resource availalability
                blocker signals resource available
                blocker gives up mutex (could happen after a few context switches too)
                --- context switch ---
                blockee receives signal and takes mutex
                blockee gives up mutex

        */


        // Signal previous boundary
        pthread_mutex_lock(b->previous_boundary->blocking_m);
        pthread_cond_signal(b->previous_boundary->blocking_cv);
        pthread_mutex_unlock(b->previous_boundary->blocking_m);
    }
}

uint8_t *calculate_next_element(uint8_t *current, common_queue_attributes *attr)
{
    if (!current || !attr)
        abort();

    // Calculate next element location (accounting for wrap-around)
    return (uint8_t *)((uintptr_t)attr->buffer_base +
                       ((uintptr_t)(current + attr->element_size) & attr->buffer_mask));
}

/* TODO - replace force_noblock with a timeout duration
Duration of zero means return null immediately instead of blocking.
Max timeout of 32-bit int in microseconds is about an hour
*/
uint8_t *boundary_get_next_active_element(queue_boundary *b, bool force_noblock)
{
    if (!b)
        abort();
    if (!b->next_element)
        abort();

    /* Only block if:
        No free elements and HEAD
        OR
        Not HEAD and (boundary issues)

    if HEAD also
        decrement free elements
    */


    // Check for conflict using next boundary
    if (((b == b->attr->head_boundary) && // This is the special HEAD boundary AND
         (!b->attr->num_free_elements))   // No free elements
        ||
        ((b != b->attr->head_boundary) &&                          // Not HEAD AND
         ((b->next_element == b->next_boundary->active_element) || // Boundary issue
          (b->next_element == b->next_boundary->next_element))))
    {
        // Check if blocking required
        if (b->blocking_cv && !force_noblock)
        {
            if (!b->blocking_m)
                abort();

            // TODO - for greater reliability, should wrap CV in while (condition) loop
            // https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032r/index.html

            // Block on next boundary
            pthread_mutex_lock(b->blocking_m);
            // TODO - should just check again here -
            //  still see efficiencies to be gaining by checking once first outside of loop before
            //  CV overhead.
            pthread_cond_wait(b->blocking_cv, b->blocking_m);
            pthread_mutex_unlock(b->blocking_m);

            // Do another error check just for debugging purposes
            if (((b == b->attr->head_boundary) && // This is the special HEAD boundary AND
                 (!b->attr->num_free_elements))   // No free elements
                ||
                ((b->next_element == b->next_boundary->active_element) || // Boundary issue
                 (b->next_element == b->next_boundary->next_element)))
            {
                abort();
            }
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
    if (b->active_element)
        boundary_done_with_active_element(b);
    // Some redundancies in above function, but worth it to avoid code dulpication

    // No need for data protection mutex if active element set before advancing next.
    // Previous boundary will be effectively blocked

    // Advance to next element
    b->active_element = b->next_element;

    if (b == b->attr->head_boundary) // This is the special HEAD boundary
    {
        // Mutext protection only needed for modifying num_free_elements.
        // Not protection needed for reading.
        // Only reader who cares about low number is head, which is also the one decrementing.
        // Tail, which is incrementing, does not care about the value.
        pthread_mutex_lock(&b->attr->data_protection_m);
        b->attr->num_free_elements--;
        pthread_mutex_unlock(&b->attr->data_protection_m);
    }

    // Set next element location (accounting for wrap-around)
    b->next_element = calculate_next_element(b->next_element, b->attr);

    return b->active_element;
}

queue_status common_queue_init(common_queue_attributes *attr, size_t element_size, size_t total_elements)
{
    // Must have valid pointer and non-zero element size and num
    if (!attr || !element_size || !total_elements)
        return QUEUE_FAILURE;

    element_size = pow2_roundup(element_size);
    total_elements = pow2_roundup(total_elements);

    size_t total_size = element_size * total_elements;

    // Allocate total memory on alligned address
    attr->buffer_base = aligned_alloc(total_size, total_size);

    // Check that alloc succeeded
    if (!attr->buffer_base)
        return QUEUE_FAILURE;

    attr->element_size = element_size;
    attr->buffer_mask = (uintptr_t)(total_size - 1);
    attr->num_free_elements = total_elements;

    return QUEUE_SUCCESS;
}

queue_status common_queue_destroy(common_queue_attributes *attr)
{
    // Not a critical check, but likely points to an error elsewhere
    if (!attr->buffer_base)
        return QUEUE_FAILURE;

    free(attr->buffer_base);
    attr->buffer_base = NULL;

    attr->buffer_mask = 0;
    attr->element_size = 0;
    attr->num_free_elements = 0;

    return QUEUE_SUCCESS;
}
