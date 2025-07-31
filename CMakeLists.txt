cmake_minimum_required(VERSION 3.13)

project(HttpServerInC)

file(GLOB_RECURSE SRC_FILES src/*.c)
file(GLOB_RECURSE HEADER_FILES inc/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_executable(http-server ${SRC_FILES} ${HEADER_FILES})
