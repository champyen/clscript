cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB csutil_src
        "*.h" 
        "*.cpp" )

add_executable(csutil ${csutil_src})
target_link_libraries(csutil clscript)
