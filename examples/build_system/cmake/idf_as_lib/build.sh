#!/bin/bash
#
# Build this example, which does not use that standard IDF app template. See README.md for
# more information about the build and how to run this example on the target once built.

mkdir build; cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DIDF_TARGET=esp32
cmake --build .