cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB test_src
    "*.h" 
    "*.cpp" )

add_executable(test ${test_src})
target_link_libraries(test clscript)

