# Build system v2 variant of the g1_components test app. The component lists,
# the closure check and the main component are shared with the build system v1
# variant in ../g1_components, so the two cannot drift apart.
cmake_minimum_required(VERSION 3.22)

set(g1_components_dir "${CMAKE_CURRENT_LIST_DIR}/../g1_components")

include(${g1_components_dir}/g1_setup.cmake)

# main lives in the build system v1 variant of this app
set(EXTRA_COMPONENT_DIRS "${g1_components_dir}/main")

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

project(g1_components_cmakev2 C CXX ASM)

idf_project_default()

include(${g1_components_dir}/g1_check.cmake)
