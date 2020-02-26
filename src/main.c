#include "main.h"

/*
 * parse parameters
 * default port 80
 */
int main(int argc, char **argv) {

    // check for param -> port
    char *port = "80";
    if (argc > 1)
        port = argv[0];

    // start http server
    http_serve(port);

    return 0;
}

void http_serve(const char *port) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int slot = 0;


    /*
     * default value for clients files descriptors
     * free = -1
    */
    int i;
    for (i = 0; i < max_client; i++)
        clients[i] = -1;

    /*
     * TODO Create socket and fill serverfd variable
     */
    bind_server_socket(port);

    while(running) {
        // TODO select() or fork() requests
    }


}

void bind_server_socket(const char *port) {
    struct addrinfo socket_options, *addrs, *addr;

    printf("[bind_server_socket] - starting server ...");

    /*
     * get bindable socket
     * http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
     * ai_flags = AI_PASSIVE  allow to bind the socket to accept requests
     */
    memset(&socket_options, 0, sizeof(socket_options));
    socket_options.ai_family = AF_INET;
    socket_options.ai_socktype = SOCK_STREAM;
    socket_options.ai_flags = AI_PASSIVE;

    /*
     * fill addrs with all available ipv4 addr
     */
    if (getaddrinfo(NULL, port, &socket_options, &addrs) != 0) {
        perror ("[bind_server_socket] - failed to get addrs");
        exit(1);
    }

    /*
     * for iteration on sockets linked list elements
     * bind sockets to port
     */
    for(addr = addrs; addr != NULL ; addr=addrs->ai_next) {
        int option = 1;
        serverfd = socket(addr->ai_family, addr->ai_socktype, 0); // default protocol
        setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // allow ot reuse same port
        if (serverfd == -1) continue; // failed to create socket
        if (bind(serverfd, addr->ai_addr, addr->ai_addrlen) == 0) break; // success
    }

    // check if we could bind a socket
    if (addr == NULL) {
        perror("[bind_server_socket] - failed to bind to port");
        exit(1);
    }

    // free addrs linked list
    freeaddrinfo(addrs);

    // start listening for request
    if ( listen (serverfd, max_pending_connection) != 0 ) {
        perror("[bind_server_socket] - failed to listen on serverfd");
        exit(1);
    }

    printf("[bind_server_socket] - server started");

}