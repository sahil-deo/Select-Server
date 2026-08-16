cmake_minimum_required(VERSION 3.10)

project(SelectServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)