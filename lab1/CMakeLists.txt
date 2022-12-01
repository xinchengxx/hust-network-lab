cmake_minimum_required(VERSION 3.21)
project(webserver)

set(CMAKE_CXX_STANDARD 17)

#cmake 也是逐行进行的-> include_directories should be ahead of add_subdirectory
include_directories(include)
add_subdirectory(src)



