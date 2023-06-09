//
// Created by Ümit Kara on 9.06.2023.
//

#ifndef HTTPSERVER_HTTP_H
#define HTTPSERVER_HTTP_H

#include <stdbool.h>

// Structure to represent an HTTP request
typedef struct {
    char method[8];     // GET, POST, etc.
    char path[256];     // /index.html, /images/logo.png, etc.
    char version[16];   // HTTP/1.1, HTTP/2.0, etc.
} HttpRequest;

// Function to parse an HTTP request
bool parse_http_request(const char *request, HttpRequest *http_request);

// Function to send an HTTP response
void send_http_response(int client_socket, const char *status_code, const char *content_type, const char *body, int body_length);

#endif //HTTPSERVER_HTTP_H
