/**
 * @brief Functions for socket handling
 * 
 * @file socketserver.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */
#include "socketserver.h"

/*
Functions to 
create a socket  and starts a listner and listen for client requests
Cleanup and free resources once done -- unlink,close
accept requests from client
send responses to client
thread function that wraps up all these and can 

*/

int create_socket_server(void)
{
    //TODO : Change to IP socket
    // socket create function    
    int socket_fd;
    struct sockaddr_un name;
    
    /* Create the socket. */
    socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);

    if(socket_fd<0) 
    {
        perror("Socket creation Failed");
        return -1;
    }
    
    /* Indicate that this is a server. */
    name.sun_family = AF_LOCAL;
    strcpy (name.sun_path, SOCKET_NAME);

    if(bind (socket_fd,(struct sockaddr*) &name, sizeof(struct sockaddr_un))<0)
    {
        perror("Bind Failed");
        return -1;
    }

    /* Listen for connections. */
    if(listen (socket_fd, 2)<0)
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

    /* Accept a connection. */
    client_socket_fd = accept (socket_fd, (struct sockaddr*)&client_name, &client_name_len);

    if(client_socket_fd<0)
    {
        perror("Accept Failed");
        return -1;
    }

    return client_socket_fd;
}


void delete_socket(int socket_fd)
{
    close (socket_fd);
    unlink (SOCKET_NAME);

}


/*
int main()
{               


    
    
    // Receive message from client
    
    printf("In process [%d] Receiving message\n",getpid());
    recv(client_socket_fd,rec_message1,sizeof(message_data_t),0);
    rec_message1->user_string=malloc(rec_message1->length);
    recv(client_socket_fd,rec_message1->user_string,rec_message1->length,0);
    

    // Send message to client
    
    send_message1=createMessage("Process1 to 2",0);
    print_message(send_message1);
    send(client_socket_fd,send_message1,sizeof(message_data_t),0);
    send(client_socket_fd,send_message1->user_string,send_message1->length,0);
    

  
    
}
*/