cmake_minimum_required(VERSION 3.22)

include(${IDF_PATH}/tools/cmakev2/idf.cmake)

project(${IDF_DEFAULT_PROJECT_NAME} C CXX ASM)

include(IDFULPProject)

idf_project_init()
ulp_prepare_build()

idf_build_library(ulp_shared_library
                  COMPONENTS ulp_shared)

idf_build_executable(add
                     COMPONENTS ulp_add_main
                     MAPFILE_TARGET add_mapfile)
target_link_libraries(add PRIVATE ulp_shared_library)

idf_build_executable(multiply
                     COMPONENTS ulp_multiply_main
                     MAPFILE_TARGET multiply_mapfile)
target_link_libraries(multiply PRIVATE ulp_shared_library)
