# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/toolchain.cmake)
project(idf_project ASM C CXX)

include($ENV{IDF_PATH}/idf.cmake)

add_executable(hello_world.elf
  main/hello_world_main.c)

target_link_libraries(hello_world.elf ${COMPONENT_LIBRARIES})
