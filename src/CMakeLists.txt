cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB clscript_src
    "*.h"
    "*.cpp"
)

add_library(clscript SHARED ${clscript_src})
target_link_libraries(clscript OpenCL)
