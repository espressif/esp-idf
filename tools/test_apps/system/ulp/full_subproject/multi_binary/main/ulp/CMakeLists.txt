cmake_minimum_required(VERSION 3.22)

include(${IDF_PATH}/components/ulp/cmake/ulp_project.cmake)

project(${IDF_DEFAULT_PROJECT_NAME} C CXX ASM)

ulp_project_init()

ulp_build_executable(add
                     COMPONENTS ulp_add_main ulp_shared
                     MAPFILE_TARGET add_mapfile)
ulp_build_binary(add)

ulp_build_executable(multiply
                     COMPONENTS ulp_multiply_main ulp_shared
                     MAPFILE_TARGET multiply_mapfile)
ulp_build_binary(multiply)
