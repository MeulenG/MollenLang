cmake_minimum_required(VERSION 3.10)

project(MollenHelper VERSION 1.0 DESCRIPTION "MollenHelpers")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(Helper Helper.cpp Helper.h)