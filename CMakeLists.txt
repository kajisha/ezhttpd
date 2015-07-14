cmake_minimum_required(VERSION 3.2.3)
project(ezhttpd)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wall -g")

add_subdirectory(src)

add_executable(server src/main.c)
target_link_libraries(server ezhttpd)
