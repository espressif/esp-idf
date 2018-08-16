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

cd "$IDF_PATH"

# Only list the "main" IDF repo, don't check any files in submodules (which may contain
# third party CMakeLists.txt)
 git ls-tree --full-tree --name-only -r HEAD | grep -v "/third_party/" | grep "^CMakeLists.txt$\|\.cmake$" \
    | xargs cmakelint --linelength=120 --spaces=4


