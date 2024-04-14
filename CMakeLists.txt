cmake_minimum_required(VERSION 3.10)

project(MollenLang VERSION 1.0 DESCRIPTION "MollenLang")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(Lexer)

add_executable(MollenLang main.cpp)

target_link_libraries(MollenLang PUBLIC Lexer)

target_include_directories(MollenLang PUBLIC "${PROJECT_BINARY_DIR}" "include")

target_compile_options(MollenLang PRIVATE -Wall -Wextra)