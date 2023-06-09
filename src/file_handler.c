//
// Created by Ümit Kara on 9.06.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"

#define WEB_ROOT "D:\\Projects\\C\\HTTPServer\\"

bool read_file(const char *file_path, char **buffer, long *length) {
    char *full_path = malloc(strlen(file_path) + strlen(WEB_ROOT) + 1);
    strcpy(full_path, WEB_ROOT);

    char *p = file_path;
    while (*p) {
        if (*p == '/') *p = '\\';
        p++;
    }

    strcat(full_path, file_path);
    FILE *file = fopen(full_path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", file_path);
        return false;
    }

    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = malloc(*length + 1);
    if (*buffer == NULL) {
        fprintf(stderr, "Error allocating memory for file %s\n", file_path);
        fclose(file);
        return false;
    }

    fread(*buffer, *length, 1, file);
    fclose(file);

    (*buffer)[*length] = '\0';

    return true;
}

const char *get_mime_type(const char *file_path) {
    const char *last_dot = strrchr(file_path, '.');
    if (last_dot == NULL || last_dot == file_path) {
        return "application/octet-stream";
    }

    if (strcmp(last_dot, ".html") == 0) return "text/html";
    if (strcmp(last_dot, ".css") == 0) return "text/css";
    if (strcmp(last_dot, ".js") == 0) return "application/javascript";
    if (strcmp(last_dot, ".json") == 0) return "application/json";
    if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
    if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(last_dot, ".png") == 0) return "image/png";
    if (strcmp(last_dot, ".gif") == 0) return "image/gif";
    if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
    if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";

    return NULL;
}