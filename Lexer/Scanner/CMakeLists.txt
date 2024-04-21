cmake_minimum_required(VERSION 3.10)

project(MollenScanner VERSION 1.0 DESCRIPTION "MollenScanner")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(Scanner Scanner.cpp Scanner.h)