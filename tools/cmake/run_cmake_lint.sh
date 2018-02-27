#!/bin/bash
#
# Run cmakelint on all cmake files in IDF_PATH (except third party)
#
# cmakelint: https://github.com/richq/cmake-lint
#
# NOTE: This script makes use of features in (currently unreleased)
# cmakelint >1.4. Install directly from github as follows:
#
# pip install https://github.com/richq/cmake-lint/archive/058c6c0ed2536.zip
#

if [ -z "${IDF_PATH}" ]; then
    echo "IDF_PATH variable needs to be set"
    exit 3
fi

# exclusions include some third-party directories which contain upstream
# CMakeLists files
find ${IDF_PATH} \
     -name build -prune \
     -o -name third_party -prune \
     \
     -o -name 'nghttp2' -prune \
     -o -name 'cJSON' -prune \
     -o -name 'Findsodium.cmake' -prune \
     \
     -o -name CMakeLists.txt -print0 \
     -o -name '*.cmake' -print0 \
    | xargs -0 cmakelint --linelength=120 --spaces=4


