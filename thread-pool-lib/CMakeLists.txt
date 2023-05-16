cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME threadPool)
project(${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/threadPool/include)

add_subdirectory(threadPool)
add_subdirectory(unitTests)