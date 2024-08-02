cmake_minimum_required(VERSION 3.10)

project(MollenLang VERSION 1.0 DESCRIPTION "MollenLang")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


add_subdirectory(Lexer)

add_subdirectory(Parser)

add_subdirectory(Ast)

add_executable(MollenLang main.cpp)

target_link_libraries(MollenLang PUBLIC Ast Scanner Parser Helper Lexer)

target_include_directories(MollenLang PUBLIC "${PROJECT_BINARY_DIR}" "include")

target_compile_options(MollenLang PRIVATE -Wall -Wextra)