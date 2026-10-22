cmake_minimum_required(VERSION 3.22)

if(IDF_BUILD_V2)
    include(${CMAKE_CURRENT_LIST_DIR}/CMakeLists_v2.txt)
    return()
endif()

# The main build enables ccache inside tools/cmake/project.cmake.
# This ULP build does not include that file, so project() below
# leaves ccache off. Turn it on here before project() runs.
# ASM compilation starts later and uses this launcher too.
if(CCACHE_ENABLE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set(CMAKE_C_COMPILER_LAUNCHER ccache)
        set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
        set(CMAKE_ASM_COMPILER_LAUNCHER ccache)
    else()
        message(WARNING "enabled ccache in build but ccache program not found")
    endif()
endif()

include(${IDF_PATH}/tools/cmake/idf.cmake)
project(${ULP_APP_NAME})
add_executable(${ULP_APP_NAME})

include(IDFULPProject)

ulp_apply_default_options(${ULP_APP_NAME})
ulp_apply_default_sources(${ULP_APP_NAME})
ulp_add_build_binary_targets(${ULP_APP_NAME} PREFIX ${ULP_VAR_PREFIX})
