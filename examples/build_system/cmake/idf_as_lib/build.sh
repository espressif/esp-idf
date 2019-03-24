#!/bin/bash
rm -rf build && mkdir build && cd build
cmake .. 
cmake --build .