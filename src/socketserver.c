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
    int socket_fd, opt = 1;
    struct sockaddr_in name;

    /* Create the socket. */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        perror("Socket creation Failed");
        return -1;
    }

     // Forcefully attaching socket to the port 8080
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return -1;
        //exit(EXIT_FAILURE);
    }
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons( PORT );

    //unlink(SOCKET_NAME);
    /*if(inet_pton(AF_INET,IP_ADDRESS, &name.sin_addr)<=0) 
    {
        perror("\nInvalid address/ Address not supported \n");
        exit(1);
    }*/


    if (bind(socket_fd, (struct sockaddr *)&name, sizeof(name)) < 0)
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
    //unlink(SOCKET_NAME);
    printf("Socket Closed\n");
}


void *socket_thread(void *thread_param)
{
    thread_param_t *p1;
    p1 = (thread_param_t *)thread_param;

    int socket_fd, client_fd;
    logged_data_t message;
    socket_fd = create_socket_server();

    enable_logging_in_thread(p1->logger);
    log_printf("Socket thread Started\n");

    while (p1->keep_thread_alive)
    {
        log_printf("Socket Task: Waiting for connection\n");
        client_fd = accept_connection(socket_fd);
        log_printf("Socket Task: Connection accepted\n");
        
        // This has to be replaced with request response code
        
        recv(client_fd, &message, sizeof(logged_data_t), 0);
        log_printf("Socket Task: Request for %s at %ld from %d\n",
            data_header_type_strings[message.type]
            ,message.req_time,message.origin);
        
        // To be changed
        message.temperature.value=25.0;
        message.res_time=message.req_time+1;
        
        send(client_fd,&message,sizeof(message),0);
        log_printf("Socket Task: Responded with %lf at %ld\n"
            ,message.temperature.value,message.res_time);
        /*
        Send this to main and 
        get response from correct task with the response      
        */
        //log_printf(); 
        //usleep(100);

    }
    
    delete_socket(socket_fd);

    log_printf("Exiting socket thread\n");

    exit(0);
}

//    send(client_socket_fd,send_message1,sizeof(message_data_t),0);