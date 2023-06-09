//
// Created by Ümit Kara on 9.06.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>
#include <io.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
#include <winsock2.h>
#include "server.h"
#include "thread_pool.h"
#include "http.h"
#include "file_handler.h"

#define MAX_REQUEST_SIZE 2048

typedef struct {
    int client_socket;
} ClientTaskData;

void handle_client_request(void *data) {
    ClientTaskData *client_task_data = (ClientTaskData *) data;
    int client_socket = client_task_data->client_socket;
    free(client_task_data);

    char request_buffer[MAX_REQUEST_SIZE];
    // ssize_t bytes_received = recv(client_socket, request_buffer, sizeof(request_buffer) - 1, 0);
    ssize_t bytes_received = recv(client_socket, request_buffer, sizeof(request_buffer) - 1, 0);
     if (bytes_received <= 0) {
        fprintf(stderr, "Error reading request.\n");
        close(client_socket);
        return;
    }
    request_buffer[bytes_received] = '\0';

    HttpRequest http_request;
    if (!parse_http_request(request_buffer, &http_request)) {
        send_http_response(client_socket, "400 Bad Request", "text/plain", "Bad Request", 11);
        close(client_socket);
        fprintf(stderr, "Error parsing request.\n");
        return;
    }

    if (strcmp(http_request.method, "GET") == 0) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "wwroot%s", http_request.path);

        char *file_buffer;
        long file_length;
        if (read_file(full_path, &file_buffer, &file_length)) {
            const char *mime_type = get_mime_type(http_request.path);
            send_http_response(client_socket, "200 OK", mime_type, file_buffer, file_length);
            printf("%s %s %s\n", http_request.method, http_request.path, "200 OK");
            free(file_buffer);
        } else {
            send_http_response(client_socket, "404 Not Found", "text/plain", "File not found", 13);
        }
    } else {
        send_http_response(client_socket, "405 Method Not Allowed", "text/plain", "Method not allowed", 17);
    }

    close(client_socket);
}

bool start_http_server(int port) {
    //int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (server_socket < 0) {
        fprintf(stderr, "Error creating socket.\n");
        close(server_socket);
        return false;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

//    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
//        fprintf(stderr, "Error binding socket.\n");
//        close(server_socket);
//        return false;
//    }
//
//    if (listen(server_socket, 5) < 0) {
//        fprintf(stderr, "Error listening socket.\n");
//        close(server_socket);
//        return false;
//    }
//
//    ThreadPool *thread_pool = thread_pool_create(4);
//    if (thread_pool == NULL) {
//        fprintf(stderr, "Error creating thread pool.\n");
//        close(server_socket);
//        return false;
//    }
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Error binding socket.\n");
        close(server_socket);
        return false;
    }

    if (listen(server_socket, 10) == -1) {
        fprintf(stderr, "Error listening socket.\n");
        close(server_socket);
        return false;
    }

    ThreadPool *thread_pool = thread_pool_create(4);
    if (!thread_pool) {
        fprintf(stderr, "Error creating thread pool.\n");
        close(server_socket);
        return false;
    }

    printf("Server started on port %d.\n", port);

    while (1) {
        struct sockaddr_in client_address;
        int client_address_length = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_length);

        if (client_socket == -1) {
            fprintf(stderr, "Error accepting client.\n");
            continue;
        }

        ClientTaskData *client_task_data = malloc(sizeof(ClientTaskData));
//        if (client_task_data == NULL) {
//            fprintf(stderr, "Error allocating memory for client task data.\n");
//            close(client_socket);
//            continue;
//        }
        client_task_data->client_socket = client_socket;

        if (!thread_pool_add_task(thread_pool, handle_client_request, client_task_data)) {
            fprintf(stderr, "Error adding task to thread pool.\n");
            close(client_socket);
            free(client_task_data);
        }
    }

    thread_pool_destroy(thread_pool);
    close(server_socket);
    return true;
}