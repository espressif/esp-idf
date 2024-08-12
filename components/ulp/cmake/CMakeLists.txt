cmake_minimum_required(VERSION 3.16)

include(${IDF_PATH}/tools/cmake/idf.cmake)
project(${ULP_APP_NAME})
add_executable(${ULP_APP_NAME})

include(IDFULPProject)

ulp_apply_default_options(${ULP_APP_NAME})
ulp_apply_default_sources(${ULP_APP_NAME})
ulp_add_build_binary_targets(${ULP_APP_NAME})
