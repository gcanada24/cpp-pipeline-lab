cmake_minimum_required(VERSION 3.20)
project(cpp_pipeline_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W4 /permissive- /utf-8)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Every folder lessons/NN-name/ with a main.cpp becomes target lessonNN.
# All .cpp files in that folder are compiled into it; common/ is on the include path.
file(GLOB LESSON_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/lessons/*")
foreach(dir ${LESSON_DIRS})
  if(EXISTS "${dir}/main.cpp")
    get_filename_component(name "${dir}" NAME)
    string(SUBSTRING "${name}" 0 2 num)
    file(GLOB srcs CONFIGURE_DEPENDS "${dir}/*.cpp")
    add_executable(lesson${num} ${srcs})
    target_include_directories(lesson${num} PRIVATE "${CMAKE_SOURCE_DIR}/common")
  endif()
endforeach()
