#This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_tee_pms_cpu_intr)

include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
message(STATUS "Checking tee/apm registers are not read-write by half-word")
check_register_rw_half_word(SOC_MODULES "*tee"       # tee, lp_tee
                                        "*apm *apm0" # hp_apm, lp_apm, cpu_apm, lp_apm0
                            HAL_MODULES "apm")
