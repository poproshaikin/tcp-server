cmake_minimum_required(VERSION 3.30)
project(client)

set(CMAKE_C_STANDARD 17)

add_executable(client main.c)
target_include_directories(client PRIVATE include)