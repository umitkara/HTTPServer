//
// Created by Ümit Kara on 9.06.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        // fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        // return EXIT_FAILURE;
        argv[1] = "8080";
    }

    int port = atoi(argv[1]);
    if (port <= 0) {
        fprintf(stderr, "Invalid port number\n");
        return EXIT_FAILURE;
    }

    if (!start_http_server(port)) {
        fprintf(stderr, "Failed to start HTTP server\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
