/**
 * @file queue_test.c
 * @brief Functions to test traditional queue and bidirectional queue
 * @author Miles Frain
 * @author Aakash Kumar
 * @version 1
 * @date 2018-02-20
 */

#if 0
#include <stdio.h>

#include "single_ended_queue.h"
#include "bidirectional_queue.h"

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

int main()
{
    // Check valid powers of 2.
    // Should move this to separate test
    for (size_t s = 0; s < 10; s++)
    {
        printf("%zu %zu\n", s, pow2_roundup(s));
    }

    bdqueue *myq = malloc(sizeof(bdqueue));

    if (bdqueue_init(myq, 3, 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }

    printf("head %p\n", myq->attr.buffer_base);
    for (int i = 0; i < 10; i++)
    {
        printf("%d %p\n", i, bdqueue_next_empty_request(myq));
    }

    bdqueue_destroy(myq);

    // Testing above identical code again to see if destory worked
    if (bdqueue_init(myq, 3, 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }

    printf("head %p\n", myq->attr.buffer_base);
    for (int i = 0; i < 10; i++)
    {
        printf("%d %p\n", i, bdqueue_next_empty_request(myq));
    }

    bdqueue_destroy(myq);

    if (bdqueue_init(myq, sizeof(example_msg), 5) == QUEUE_FAILURE)
    {
        printf("failed init\n");
    }
    printf("head %p\n", myq->attr.buffer_base);
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

    return 0;
}
#endif