cmake_minimum_required(VERSION 3.30)
project(tcp-server)

set(CMAKE_C_STANDARD 17)

add_executable(tcp-server main.c server.c
        conn_pool.h
        conn_pool.c
        utils.c
        utils.h)
