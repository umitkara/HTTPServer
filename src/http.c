//
// Created by Ümit Kara on 9.06.2023.
//

#include <stdio.h>
#include <string.h>
// #include <unistd.h>
#include <io.h>
#include <winsock2.h>
#include "http.h"


bool parse_http_request(const char *request, HttpRequest *http_request) {
    if (sscanf(request, "%s %s %s", http_request->method, http_request->path, http_request->version) != 3) {
        return false;
    }
    return true;
}

void send_http_response(int client_socket, const char *status_code, const char *content_type, const char *body, int body_length) {
    char header[1024];
    sprintf(header, "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", status_code, content_type, body_length);

    // Send header
    // send(client_socket, header, strlen(header), 0);
    send(client_socket, header, strlen(header), 0);


    // Send body
    if (body != NULL && body_length > 0) {
        // send(client_socket, body, body_length, 0);
        send(client_socket, body, body_length, 0);
    }
}