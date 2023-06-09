cmake_minimum_required(VERSION 3.25)
project(HTTPServer C)

set(CMAKE_C_STANDARD 23)

add_executable(HTTPServer src/main.c src/server.c src/server.h src/http.c src/http.h src/file_handler.c src/file_handler.h src/thread_pool.c src/thread_pool.h)

target_link_libraries(HTTPServer PRIVATE -lws2_32)