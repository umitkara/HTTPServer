//
// Created by Ümit Kara on 9.06.2023.
//

#ifndef HTTPSERVER_FILE_HANDLER_H
#define HTTPSERVER_FILE_HANDLER_H

#include <stdbool.h>

// Function to read a file from the disk
bool read_file(const char *file_path, char **buffer, long *length);

// Function to determine the MIME type of a file based on its extension
const char *get_mime_type(const char *file_path);

#endif //HTTPSERVER_FILE_HANDLER_H
