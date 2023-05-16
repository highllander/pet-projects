cmake_minimum_required(VERSION 3.15)

add_library(threadPool STATIC   ${CMAKE_CURRENT_SOURCE_DIR}/src/ThreadPool.cpp 
                                ${CMAKE_CURRENT_SOURCE_DIR}/include/ThreadPool.h)