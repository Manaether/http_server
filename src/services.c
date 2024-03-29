//
// Created by florian.aubin on 05/03/2020.
//


#include "services.h"
#include <errno.h>

extern int errno;

void route(Request *request) {

    if (strcmp(request->uri, "/home") == 0) {
        Response *resp = http_create_response(request->clientfd);
        resp->response_code = 200;

        const unsigned char *hello = "Hello World ! ";
        resp->content.content = malloc(strlen(hello)) + 1;
        resp->content.content_length = (int) strlen(hello);
        strcpy(resp->content.content, hello);

        http_send_response(request, resp);

        return;
    }

    unsigned char *uri = getFullUri(request->uri);
    enum pathType type = getServiceIsAvailable(uri);

    if (type != isNothing) {

        Response *resp = http_create_response(request->clientfd);
        resp->response_code = 200;

        char *buffer = NULL;

        switch (type) {
            case isFile:
                getFileContent(uri, resp);

                unsigned char *ext = strrchr(uri, '.');
                if (ext != NULL && strcmp(ext, ".png") == 0) {
                    resp->content.content_type = MIME_PNG;
                }
                if (ext != NULL && (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)) {
                    resp->content.content_type = MIME_JPG;
                }


                break;
            case isDirectory:
                buffer = getDirectoryContent(uri, request->uri);
                resp->content.content = malloc(strlen(buffer) + 1);
                resp->content.content_length = (int) strlen(buffer);
                strcpy(resp->content.content, buffer);
                break;
        }

        if (buffer != NULL)
            free(buffer);
        free(uri);
        http_send_response(request, resp);

        return;
    }

    free(uri);
    // last fallback
    Response *resp = http_create_response(request->clientfd);
    resp->response_code = 404;

    const unsigned char *notfound = "404 Page Not Found";
    resp->content.content = malloc(strlen(notfound) + 1);
    resp->content.content_length = (int) strlen(notfound);
    strcpy(resp->content.content, notfound);

    http_send_response(request, resp);

    return;
}
