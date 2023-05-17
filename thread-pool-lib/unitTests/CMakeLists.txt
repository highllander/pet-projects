cmake_minimum_required(VERSION 3.15) 

project (${CMAKE_PROJECT_NAME}_UT)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(${PROJECT_NAME} testThreadPool.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE threadPool ${Boost_LIBRARIES})

enable_testing()

add_test(
    NAME AllTests
    COMMAND ${PROJECT_NAME} --log_level=test_suite
)
