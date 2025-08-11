cmake_minimum_required(VERSION 3.22)

# placeholder_before_include_project_cmake

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

# placeholder_after_include_project_cmake

# FIXME: Replace the following with idf_project_default when it becomes
# available.
project(buildv2_test_app C CXX ASM)

__generate_project_info()
