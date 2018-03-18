/**
 * @brief Functions for socket handling
 *
 * @file socketserver.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */

#include "socketserver.h"
#include "tasks.h"

/*
Functions to
create a socket  and starts a listner and listen for client requests
Cleanup and free resources once done -- unlink,close
accept requests from client
send responses to client
thread function that wraps up all these and can stay alive till main closes it

*/

int create_socket_server(void)
{
    //TODO : Change to IP socket
    // socket create function
    int socket_fd;
    struct sockaddr_un name;

    /* Create the socket. */
    socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        perror("Socket creation Failed");
        return -1;
    }

    /* Indicate that this is a server. */
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, SOCKET_NAME);

    unlink(SOCKET_NAME);

    if (bind(socket_fd, (struct sockaddr *)&name, sizeof(struct sockaddr_un)) < 0)
    {
        perror("Bind Failed");
        return -1;
    }

    /* Listen for connections. */
    if (listen(socket_fd, 2) < 0)
    {
        perror("Listen Failed");
        return -1;
    }

    return socket_fd;
}

int accept_connection(int socket_fd)
{
    struct sockaddr_un client_name;
    socklen_t client_name_len;
    int client_socket_fd;

    client_name_len = sizeof(struct sockaddr_un);

    /* Accept a connection. */
    client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_name, &client_name_len);

    if (client_socket_fd < 0)
    {
        perror("Accept Failed");
        return -1;
    }

    return client_socket_fd;
}

void delete_socket(int socket_fd)
{
    close(socket_fd);
    unlink(SOCKET_NAME);
    printf("Socket Closed\n");
}

void *socket_thread(void *thread_param)
{
    thread_param_t *p1;
    p1 = (thread_param_t *)thread_param;

    int socket_fd, client_fd;
    logged_data_t *message;
    socket_fd = create_socket_server();
    client_fd = accept_connection(socket_fd);

    enable_logging_in_thread(p1->logger);
    log_printf("--------->Socket thread says hi\n");

    while (p1->keep_thread_alive)
    {
        // Block here until we can get a free spot in queue to write received message to.
        message = (logged_data_t*)sequeue_next_empty(p1->from_socket_q, false);

        // This has to be replaced with request response code
        // Write message received from socket directly to queue
        recv(client_fd, message, sizeof(logged_data_t), 0);
        log_printf("----->Socket Request for %s at %ld from %d\n",
               data_header_type_strings[message->type], message->req_time, message->origin);

        // Completed our write
        sequeue_done_writing(p1->from_socket_q);

        // Non-Blocking check for any new messages received from main
        while ((message = (logged_data_t*)sequeue_read_next(p1->to_socket_q, true)))
        {
            log_printf("-------->Main's response to Socket Request for %s at %ld from %d\n",
               data_header_type_strings[message->type], message->req_time, message->origin);

            // Send this response to socket client
            send(client_fd, message, sizeof(message), 0);

            sequeue_done_reading(p1->to_socket_q);
        }

        /*
        Send this to main and
        get response from correct task with the response
        */
        //log_printf();
        usleep(100);

    }

    delete_socket(socket_fd);

    exit(0);
}

//    send(client_socket_fd,send_message1,sizeof(message_data_t),0);