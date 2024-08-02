cmake_minimum_required(VERSION 3.10)

project(Codegen VERSION 1.0 DESCRIPTION "MollenCodeGen")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(Codegen Codegen.cpp Codegen.h)

target_link_libraries(Codegen)
