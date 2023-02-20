cmake_minimum_required(VERSION 3.19)
project(Parser_rstyle)

set(CMAKE_CXX_STANDARD 17)

add_executable(Parser_rstyle main.cpp parser.cpp parser.h)