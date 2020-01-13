cmake_minimum_required(VERSION 3.15)
project(Block4 C)

set(CMAKE_C_COMPILER clang)
add_executable(ntpclient ntp.c ntp.h ntpclient.c)


