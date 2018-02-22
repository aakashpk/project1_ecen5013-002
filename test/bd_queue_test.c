/*
 * From https://github.com/clibs/cmocka/blob/master/example/simple_test.c
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

static void test_bd_queue_init(void **state)
{
    // Initialize with non-exact power of 2 size
}

static void test_bd_queue_init_pow2(void **state)
{
    // Initialize with exactly power of 2 elements and size
}

static void test_bd_queue_destroy(void **state)
{
    // Not sure how to best verify destroy
}

static void test_bd_queue_requester_get_next_empty_request(void **state)
{
    // create queue
    // add a few items
    // call get_next_empty_request() a few times.
    // Verify requester write pointer increments and that returned
    // addresses increment correctly.
    // Should also verify that address rolls-over correctly
}

static void test_bd_queue_requester_done_populating_request(void **state)
{
    // Verify responder read pointer advances correctly.
    // Other pointers should not change.
    // Check for rollover correctness
}

static void test_bd_queue_responder_get_next_request(void **state)
{
    // similar to test_bd_queue_requester_get_next_empty_request()
}

static void test_bd_queue_responder_done_populating_request(void **state)
{
    // similar to test_bd_queue_requester_done_populating_request()
}

static void test_bd_queue_requester_get_next_response(void **state)
{
    // similar to test_bd_queue_requester_get_next_empty_request()
}

static void test_bd_queue_requester_done_reading_response(void **state)
{
    // similar to test_bd_queue_requester_done_populating_request()
}

/*
 * Note, need to set environment variable for tests to abort for multiple threads.
 * https://api.cmocka.org/
 *     CMOCKA_TEST_ABORT='1' ./my_threading_test
 */

static void test_bd_queue_blocking_responder_get_next_request(void **state)
{
    // This function is parent thread and acts as requester
    //    Start with empty queue
    //    Spawn child thread as responder
    //    sleep for 10ms
    //    call next_empty_request() then done_populating_request()
    //         these should unblock child responder
    //    attempt to join with child responder
    //         could use pthread_timedjoin_np() to prevent testing lockup

    // responder child thread
    //    get timestamp
    //    call and block on next_request()
    //    get another timestamp and verify at least 10ms elapsed
    //    exit thread
}

static void test_bd_queue_blocking_requester_get_next_response(void **state)
{
    // similar to test_bd_queue_blocking_responder_get_next_request()
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
