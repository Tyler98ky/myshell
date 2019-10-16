# cmake_minimum_required(VERSION <specify CMake version here>)
project(linux_shell C)

set(CMAKE_C_STANDARD 99)

add_executable(myshell myshell.c myshell.h)
