cmake_minimum_required(VERSION 3.10)

project(MollenLexer VERSION 1.0 DESCRIPTION "MollenLexer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


add_subdirectory(Scanner)
add_subdirectory(Helper)
add_subdirectory(Tokens)

add_library(Lexer Lexer.cpp Lexer.h)

target_link_libraries(Lexer PUBLIC Token)