/**
 * @brief Socket client requests
 * 
 *
 * @file socketclient.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */

#include "socketclient.h"

char *data_header_type_strings[] = {
    "Heartbeat",
    "Temperature",
    "Light",
};

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
    socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        perror("Socket creation Failed");
        exit(1);
    }

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
    strcpy(name.sun_path, SOCKET_NAME);

    // Connect the socket.
    if (connect(socket_fd, (struct sockaddr *)&name, SUN_LEN(&name)) < 0)
    {
        perror("Connect Failed");
        exit(1);
    }

    return 0;
}

int main()
{
    
    /* TODO : make the request generic using command line parameters
    extern char *optarg;
    extern int optind;
    int optret;

    // Get Log file name from command line
    // If not proper format use a default one
    optret = getopt(argc, argv, "f:");

    if (argc < 3 || optret != 'f')
    {
        printf("Usage is project1 -f logfilename\n \
        File option not proper, using %s\n", logfilename);
    }
    else
        strcpy(logfilename, optarg);
    */

    int socket_fd;

    logged_data_t message, received_message;
    socket_fd = create_socket_client();
    socket_connect(socket_fd);


    // Add external API request to socket server here
    
    message.type = TEMPERATURE;
    message.req_time = time(NULL);
    message.origin=SOCKET;

    printf("Requesting: %s at %ld\n",
        data_header_type_strings[message.type],message.req_time);

    send(socket_fd,&message, sizeof(message), 0);

    recv(socket_fd,&received_message,sizeof(logged_data_t),0);

    printf("Received %lf at %ld",
        received_message.common.value,received_message.res_time);

    close(socket_fd);
    return 0;
}
