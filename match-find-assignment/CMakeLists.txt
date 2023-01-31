cmake_minimum_required(VERSION 3.15) 

set(PROJECT_NAME MatchFind)
project(${PROJECT_NAME})

set(BOOST_LIBS program_options filesystem regex unit_test_framework)

file(GLOB_RECURSE SOURCE_HEADERS matchfind/*/*.h)
file(GLOB_RECURSE SOURCE_SRC ./matchfind/*/*.cpp)

find_package(Boost 1.40 REQUIRED COMPONENTS ${BOOST_LIBS})

include_directories(matchfind/src)

add_executable(${PROJECT_NAME} matchfind/main.cpp ${SOURCE_SRC} ${SOURCE_HEADERS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})

add_subdirectory(unitTests)