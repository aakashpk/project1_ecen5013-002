/**
 * @brief Functions for socket handling
 *
 * @file socketserver.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */

#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "message_data_type.h"

/*
Functions to
create a socket  and starts a listner and listen for client requests
receive external API requests
respond to external API requests
Cleanup and free resources once done -- unlink,close

thread function that wraps up all these and can stay alive till main closes it
*/

/**
 * @brief Create a socket object,
 * bind to an address and start listening
 *
 * @return int socket file descriptor
 */
int create_socket_server(void);

/**
 * @brief Accept a socket connection request
 * and return the scoket client file descriptor
 *
 * @param socket_fd socket server file descriptor
 * @return int socket client file descriptor
 */
int accept_connection(int socket_fd);

/**
 * @brief Creates the socket task thread
 * accepts connection and sends back data to client
 *
 * @param thread_param
 * @return int
 */
void *socket_thread(void *thread_param);

/**
 * @brief Unlinks socket and closes the socket file handlers
 *
 */
void delete_socket(int socket_fd);

#endif
