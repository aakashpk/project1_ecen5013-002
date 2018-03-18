#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "bidirectional_queue.h"
#include "single_ended_queue.h"

typedef enum {
    HEARTBEAT,
    TEMPERATURE,
    LIGHT,
} example_msg_types;

char *example_msg_type_strings[] = {
    "Heartbeat",
    "Temperature",
    "Light",
};

typedef struct
{
    example_msg_types type;
    uint8_t send_idx;
    uint8_t response_idx;
    union {
        struct
        {
            uint8_t foo;
        } heartbeat;
        struct
        {
            uint8_t temp_value;
        } temperature;
    };
} example_msg;


static void test_pow2_roundup(void **state) {
    assert_int_equal(pow2_roundup(1), 1);
    assert_int_equal(pow2_roundup(2), 2);
    assert_int_equal(pow2_roundup(3), 4);
    assert_int_equal(pow2_roundup(4), 4);
    assert_int_equal(pow2_roundup(5), 8);
    assert_int_equal(pow2_roundup(6), 8);
    assert_int_equal(pow2_roundup(7), 8);
    assert_int_equal(pow2_roundup(8), 8);
    assert_int_equal(pow2_roundup(9), 16);
}

static void test_bd_queue_init(void **state)
{
    bdqueue myq;

    // Initialize with non-exact power of 2 size
    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);
}

static void test_bd_queue_memory_allignment(void **state)
{
    bdqueue myq;
    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);

    // Testing correctness of memory allignment
    assert_int_equal((uintptr_t)myq.attr.buffer_base % (pow2_roundup(3) * pow2_roundup(5)), 0);
    assert_int_equal((uintptr_t)myq.attr.buffer_base % myq.attr.element_size, 0);
}

static void test_bd_queue_destroy(void **state)
{
    bdqueue myq;
    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);
    assert_int_equal(bdqueue_destroy(&myq), QUEUE_SUCCESS);
}

static void test_bd_queue_element_increment(void **state)
{
    bdqueue myq;
    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);

    uint8_t *pos = myq.attr.buffer_base;
    for (int i = 0; i < 8; i++)
    {
        assert_ptr_equal(bdqueue_next_empty_request(&myq), pos);
        pos += myq.attr.element_size;
    }
    assert_int_equal(bdqueue_destroy(&myq), QUEUE_SUCCESS);
}

static void test_bd_queue_element_increment_after_destroy(void **state)
{
    bdqueue myq;
    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);

    uint8_t *pos = myq.attr.buffer_base;
    for (int i = 0; i < 8; i++)
    {
        assert_ptr_equal(bdqueue_next_empty_request(&myq), pos);
        pos += myq.attr.element_size;
    }
    assert_int_equal(bdqueue_destroy(&myq), QUEUE_SUCCESS);

    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);

    pos = myq.attr.buffer_base;
    for (int i = 0; i < 8; i++)
    {
        assert_ptr_equal(bdqueue_next_empty_request(&myq), pos);
        pos += myq.attr.element_size;
    }
    assert_int_equal(bdqueue_destroy(&myq), QUEUE_SUCCESS);
}

static void test_bd_queue_null_elemnts_when_full(void **state)
{
    bdqueue myq;
    assert_int_equal(bdqueue_init(&myq, 3, 5), QUEUE_SUCCESS);

    uint8_t *pos = myq.attr.buffer_base;
    for (int i = 0; i < 8; i++)
    {
        assert_ptr_equal(bdqueue_next_empty_request(&myq), pos);
        pos += myq.attr.element_size;
    }

    // Should get null for these
    for (int i = 0; i < 8; i++)
    {
        assert_null(bdqueue_next_empty_request(&myq));
    }

    assert_int_equal(bdqueue_destroy(&myq), QUEUE_SUCCESS);
}

#if 0
    // Can have up to num_elements - 1 active requests before breaking.
    // Should be able to reach num_elements, but supporting this off-by-one limitation
    // would introduce more complexity and is not worth the effort.
    for (int i = 0; i < 6; i++)
    {
        uint8_t *data = bdqueue_next_empty_request(myq);
        printf("empty \t\tpad %d %p\n", i, data);

        example_msg *msg = (example_msg *)data;
        msg->send_idx = i;
        if (i % 2)
        {
            msg->type = HEARTBEAT;
        }
        else
        {
            msg->type = TEMPERATURE;
        }

        bdqueue_done_writing_request(myq);
    }

    for (int i = 100; i < 110; i++)
    {
        uint8_t *data;
        example_msg *msg;

        // Requester / Main Task

        data = bdqueue_next_empty_request(myq);
        printf("\nempty \t\t%d %p\n", i, data);

        msg = (example_msg *)data;
        msg->send_idx = i;
        if (i % 2)
        {
            msg->type = HEARTBEAT;
        }
        else
        {
            msg->type = TEMPERATURE;
        }
        bdqueue_done_writing_request(myq);

        // Responder / Sensor Task

        data = bdqueue_next_request(myq, false);
        printf("request \t%d %p\n", i, data);

        msg = (example_msg *)data;
        msg->response_idx = i;
        switch (msg->type)
        {
        case HEARTBEAT:
            msg->heartbeat.foo = i * 10;
            break;
        case TEMPERATURE:
            msg->temperature.temp_value = i * 10 + i;
            break;
        }
        bdqueue_done_reading_request_and_writing_response(myq);

        // Requester / Main Task
        msg = (example_msg *)bdqueue_next_response(myq, false);
        printf("response \t%d %p: %s, send_idx %u, resp_idx %u\n", i, (void *)msg,
               example_msg_type_strings[msg->type], msg->send_idx, msg->response_idx);

        bdqueue_done_reading_response(myq);
    }

    bdqueue_destroy(myq);

    myq = NULL;

    // ------- Single-ended test

    sequeue *myseq = malloc(sizeof(sequeue));

    if (sequeue_init(myseq, 3, 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }

    printf("head %p\n", myseq->attr.buffer_base);
    for (int i = 0; i < 10; i++)
    {
        printf("%d %p\n", i, sequeue_next_empty(myseq, true));
    }

    sequeue_destroy(myseq);

    // Testing above identical code again to see if destory worked
    if (sequeue_init(myseq, 3, 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }

    printf("head %p\n", myseq->attr.buffer_base);
    for (int i = 0; i < 10; i++)
    {
        printf("%d %p\n", i, sequeue_next_empty(myseq, true));
    }

    sequeue_destroy(myseq);

    if (sequeue_init(myseq, sizeof(example_msg), 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }
    printf("head %p\n", myseq->attr.buffer_base);
    // Can have up to num_elements - 1 active requests before breaking.
    // Should be able to reach num_elements, but supporting this off-by-one limitation
    // would introduce more complexity and is not worth the effort.
    for (int i = 0; i < 6; i++)
    {
        uint8_t *data = sequeue_next_empty(myseq, true);
        printf("empty \t\tpad %d %p\n", i, data);

        example_msg *msg = (example_msg *)data;
        msg->send_idx = i;
        if (i % 2)
        {
            msg->type = HEARTBEAT;
        }
        else
        {
            msg->type = TEMPERATURE;
        }

        sequeue_done_writing(myseq);
    }

    for (int i = 100; i < 110; i++)
    {
        uint8_t *data;
        example_msg *msg;

        // Writer task

        data = sequeue_next_empty(myseq, true);
        printf("\nempty \t\t%d %p\n", i, data);

        msg = (example_msg *)data;
        msg->send_idx = i;
        if (i % 2)
        {
            msg->type = HEARTBEAT;
            msg->heartbeat.foo = i * 10;
        }
        else
        {
            msg->type = TEMPERATURE;
            msg->temperature.temp_value = i * 10 + i;
        }
        sequeue_done_writing(myseq);

        // Reader task

        msg = (example_msg *)sequeue_read_next(myseq, true);
        printf("response \t%d %p: %s, send_idx %u, resp_idx %u\n", i, (void *)msg,
               example_msg_type_strings[msg->type], msg->send_idx, msg->response_idx);

        sequeue_done_reading(myseq);
    }

    sequeue_destroy(myseq);
#endif

#if 0
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
#endif

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_pow2_roundup),
        cmocka_unit_test(test_bd_queue_init),
        cmocka_unit_test(test_bd_queue_memory_allignment),
        cmocka_unit_test(test_bd_queue_destroy),
        cmocka_unit_test(test_bd_queue_element_increment),
        cmocka_unit_test(test_bd_queue_element_increment_after_destroy),
        cmocka_unit_test(test_bd_queue_null_elemnts_when_full),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
