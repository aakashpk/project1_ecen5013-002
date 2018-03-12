/**
 * @brief Socket client to make API requests to main
 * 
 * @file socketclient.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */
#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define SOCKET_NAME "mysocket"

/*
Functions for
    Connecting to socket server
    sending requests
    receiving responses
*/

int create_socket_client();

int socket_connect(int socket_fd);

#endif