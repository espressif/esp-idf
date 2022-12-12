cmake_minimum_required(VERSION 3.16)

# Here for reproducible builds, we use the CI compile options to make sure they're same.
# Use -ffile-prefix-map to map the local path prefix to ci path prefix

set(ENV{EXTRA_CFLAGS} "-Werror -Werror=deprecated-declarations -Werror=unused-variable \
    -Werror=unused-but-set-variable -Werror=unused-function -Wstrict-prototypes \
    -ffile-prefix-map=$ENV{IDF_PATH}=/builds/espressif/esp-idf")

set(ENV{EXTRA_CXXFLAGS} "-Werror -Werror=deprecated-declarations -Werror=unused-variable \
    -Werror=unused-but-set-variable -Werror=unused-function \
    -ffile-prefix-map=$ENV{IDF_PATH}=/builds/espressif/esp-idf")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_core_dump)
