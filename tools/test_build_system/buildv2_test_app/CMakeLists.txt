cmake_minimum_required(VERSION 3.22)

# placeholder_before_include_project_cmake

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

# placeholder_after_include_project_cmake

project(build_test_app C CXX ASM)

idf_project_default()

#[[
When doing idf.py add-dependency, the component manager
expects the tools/cmake/project.cmake file to be included
in the project-level CMakeLists.txt file. If it does not
find it, it assumes that that the project-root directory
is a component and crates a manifest in the project-root
directory. Hence, as a workaround, we trick the component
manager by including the tools/cmake/project.cmake file
but return early.
TODO: Remove this workaround (IDF-14072)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#]]
