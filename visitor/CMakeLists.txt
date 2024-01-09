cmake_minimum_required (VERSION 3.24)

project ("Simple CMake Template" VERSION 1.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(visitor "visitor.cpp")
