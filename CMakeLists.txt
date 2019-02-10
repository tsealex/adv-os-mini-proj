cmake_minimum_required(VERSION 3.9.6)
project(mini_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(fs_default fs_default.cpp fs_directio.cpp cc_mutex.cpp)
