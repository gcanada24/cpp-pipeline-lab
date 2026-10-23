cmake_minimum_required(VERSION 3.20)
project(cpp_pipeline_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Each week adds its own targets here, e.g.:
# add_executable(week01_takes week01/cpp/main.cpp)
