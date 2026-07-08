cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Keep COMPONENTS unset and disable minimal build so every Linux-compatible
# component is compiled and linked for this app.
idf_build_set_property(MINIMAL_BUILD OFF)

project(linux_full_build)
