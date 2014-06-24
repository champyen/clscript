project(CL_SCRIPT)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(src)

include_directories (${CL_SCRIPT_SOURCE_DIR}/src)
link_directories (${CL_SCRIPT_BINARY_DIR}/src) 

add_subdirectory(test)
add_subdirectory(csutil)
