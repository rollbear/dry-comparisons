cmake_minimum_required(VERSION 3.13)
project(dry-comparisons)
set(CMAKE_CXX_STANDARD  17)
add_executable(self_test self_test.cpp dry-comparisons.hpp)
