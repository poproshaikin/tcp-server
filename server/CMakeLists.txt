cmake_minimum_required(VERSION 3.30)
project(server)

set(CMAKE_C_STANDARD 17)

add_executable(server main.c server.c
        conn_pool.h
        conn_pool.c
        ../utils.c
        ../message.h
)

target_include_directories(server PRIVATE include)