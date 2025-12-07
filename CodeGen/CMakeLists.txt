cmake_minimum_required(VERSION 3.10)

project(Codegen VERSION 1.0 DESCRIPTION "MollenCodeGen")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(LLVM REQUIRED CONFIG)

add_library(Codegen Codegen.cpp Codegen.h)

llvm_map_components_to_libnames(llvm_libs core support)

target_link_libraries(Codegen PUBLIC ${llvm_libs})
