cmake_minimum_required(VERSION 3.3)

project(test-asio)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(asio/asio/include)
set(SOURCE_FILES main.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME}   -lpthread)