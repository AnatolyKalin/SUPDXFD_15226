cmake_minimum_required(VERSION 3.20)
project(SUPDXFD_15226 C)

set(CMAKE_C_STANDARD 99)

find_package(CURL REQUIRED)

add_executable(SUPDXFD_15226 main.c)

target_link_libraries(SUPDXFD_15226 CURL::libcurl)
