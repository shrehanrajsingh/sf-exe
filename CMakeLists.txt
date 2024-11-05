cmake_minimum_required(VERSION 3.5.0)
project(sunflower-exe VERSION 0.1.0 LANGUAGES C)

add_subdirectory(lib/csunflower)
include_directories(lib/csunflower)

target_compile_definitions(sunflower PUBLIC SF_NODEBUG)

add_executable(sunflower-exe main.c)
target_link_libraries(sunflower-exe sunflower)

