cmake_minimum_required(VERSION 3.13)
project(dry-comparisons)

if (NOT CMAKE_CXX_STANDARD)
  if (MSVC)
    set(CMAKE_CXX_STANDARD 20)
  else()
    set(CMAKE_CXX_STANDARD 17)
  endif()
endif()

add_executable(self_test dry-comparisons.hpp self_test.cpp)
