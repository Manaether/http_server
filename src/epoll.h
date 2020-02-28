//
// Created by florian.aubin on 28/02/2020.
//

#ifndef HTTP_SERVER_EPOLL_H
#define HTTP_SERVER_EPOLL_H

#include <zconf.h>
#include <ctype.h>

#define STDIN 0

enum {
    max_event = 10
};

/*
 * add non blocking flag to a file descriptor
 */
int epoll_setnonblocking(int fd);

/*
 * epoll based server
 */
void epoll_serve(const char* port);

/*
 * handle epoll client event (input/disconnect)
 */
void epoll_client_event(int eventIndex);

/*
 * handle epoll new requests
 */
void epoll_server_event(int eventIndex);

/*
 * handle stdin commands
 */
void epoll_stdin_event();

int epoll_content_length(const char* content, unsigned int content_length);

#endif //HTTP_SERVER_EPOLL_H
