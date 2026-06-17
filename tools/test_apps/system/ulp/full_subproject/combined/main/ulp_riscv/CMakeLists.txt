cmake_minimum_required(VERSION 3.22)

include(${IDF_PATH}/tools/cmakev2/idf.cmake)

project(${IDF_DEFAULT_PROJECT_NAME} C CXX ASM)

include(IDFULPProject)
ulp_project_default()
