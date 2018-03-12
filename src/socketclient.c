/**
 * @brief 
 * 
 * @file socketclient.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */


#include "socketclient.h"

int main()
{
    
    int socket_fd;
    struct sockaddr_un name;
    
    /* Create the socket. */
    socket_fd = socket (PF_LOCAL, SOCK_STREAM, 0);
    
    if(socket_fd<0) perror("Socket creation Failed");

    /* Indicate that this is a server. */
    name.sun_family = AF_LOCAL;
    strcpy (name.sun_path, SOCKET_NAME);
    
        // Connect the socket. 
        if(connect (socket_fd,(struct sockaddr*)&name, SUN_LEN (&name))<0)
        {
            perror("Connect Failed");
            exit(1);
        }

        // Send message to server
        printf("In process [%d] Sending Message- ",getpid());
        //send_message2=createMessage("Process 2 to 1",0);
        //print_message(send_message2);
        // send(socket_fd,send_message2,sizeof(message_data_t),0);
        // send(socket_fd,send_message2->user_string,send_message2->length,0);
        printf("\nIn process [%d] Message Sent\n",getpid());
        
        // Receive message from server
        printf("In process [%d] Receiving message\n",getpid());
        // recv(socket_fd,rec_message2,sizeof(message_data_t),0);
        // rec_message2->user_string=malloc(rec_message2->length);
        // recv(socket_fd,rec_message2->user_string,rec_message2->length,0);
        
       
        // Free malloced structures
        printf("\nIn process [%d] Freeing Memory\n",getpid());


        close (socket_fd);
        exit(1);     

}
