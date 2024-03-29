//
// Created by florian.aubin on 28/02/2020.
//

#ifndef HTTP_SERVER_EPOLL_H
#define HTTP_SERVER_EPOLL_H

#include <zconf.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include "http.h"

#define STDIN 0

/*
 * number of event in the epoll
 */
enum {
    max_event = 10
};

enum {
    max_worker = 10
};

/*
 * add non blocking flag to a file descriptor [why does the socket become unix ?] FuMyLi
 * this cause a failure to accept() if sockaddr is not sockaddr_un
 * sockaddr_in -> sockaddr_un
 */
int epoll_setnonblocking(int fd);

/*
 * epoll based server
 */
void epoll_serve(const char *interface, const char* port);

/*
 * epoll for slave processes
 */
void epoll_slave();

/*
 * handle epoll client event (input/disconnect)
 */
void epoll_client_event(int eventIndex);

/*
 * handle epoll new requests
 */
int epoll_server_event();

/*
 * handle stdin commands
 */
void epoll_stdin_event();

/*
 * handle worker start and restart
 */
void epoll_manager();

/*
 * handle worker epoll
 */
void epoll_worker() __attribute__((noreturn));

int epoll_content_length(const char* content, unsigned int content_length);

#endif //HTTP_SERVER_EPOLL_H

