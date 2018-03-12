/**
 * @brief 
 * 
 * @file socketclient.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */


#include "socketclient.h"

/**
 * @brief Open socket client file
 * and return file descriptor
 * 
 * @return int socket flie descriptor
 */
int create_socket_client()
{
    int socket_fd;
        
    /* Create the socket. */
    socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);
    
    if(socket_fd<0) perror("Socket creation Failed");

    return socket_fd;
}

/**
 * @brief Connect to socket server,
 * and return socket client file descriptor
 * 
 * @param socket_fd 
 * @return int socket client file descriptor
 */
int socket_connect(int socket_fd)
{
    struct sockaddr_un name;
   
    name.sun_family = AF_LOCAL;
    strcpy (name.sun_path, SOCKET_NAME);
    
    // Connect the socket. 
    if(connect (socket_fd,(struct sockaddr*)&name, SUN_LEN (&name))<0)
    {
        perror("Connect Failed");
        return -1;
    }

    return 0;

}


int main()
{
    int socket_fd;
    char message[20] = "Hello";
    socket_fd=create_socket_client();
    socket_connect(socket_fd);

    // Add external API request to socket server here
    printf("Sending: %s\n",message);

    send(socket_fd,message,sizeof(message),0);
   
    close (socket_fd);
    return 0;
        

}
