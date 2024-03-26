#!/usr/bin/env bash

set -u
set -e

echo "Checking if IDF version in esp_idf_version.h matches 'git describe' output"

function get_ver_from_header() {
    # Get the 3rd word from '#define ESP_IDF_VERSION_X <number>' line.
    grep -E "^#define ${1}" components/esp_common/include/esp_idf_version.h | awk '{print $3;}'
}

function get_ver_from_cmake() {
    grep -E "^set\(${1}" tools/cmake/version.cmake | sed -En "s/set\(${1} ([0-9])\)/\1/p"
}

header_ver_major=$(get_ver_from_header ESP_IDF_VERSION_MAJOR)
header_ver_minor=$(get_ver_from_header ESP_IDF_VERSION_MINOR)
header_ver_patch=$(get_ver_from_header ESP_IDF_VERSION_PATCH)
version_from_header="${header_ver_major}.${header_ver_minor}.${header_ver_patch}"

cmake_ver_major=$(get_ver_from_cmake IDF_VERSION_MAJOR)
cmake_ver_minor=$(get_ver_from_cmake IDF_VERSION_MINOR)
cmake_ver_patch=$(get_ver_from_cmake IDF_VERSION_PATCH)
version_from_cmake="${cmake_ver_major}.${cmake_ver_minor}.${cmake_ver_patch}"

git_desc=$(git describe --exclude 'qa-test-*')
git_desc_regex="^v([0-9]+)\.([0-9]+)(\.([0-9]+))?.*$"
if [[ ! ${git_desc} =~ ${git_desc_regex} ]]; then
    echo "Could not determine the version from 'git describe' output: ${git_desc}"
    exit 1
fi
version_from_git="${BASH_REMATCH[1]}.${BASH_REMATCH[2]}.${BASH_REMATCH[4]:-0}"

echo "From esp_idf_version.h: ${version_from_header}"
echo "From version.cmake: ${version_from_cmake}"
echo "From git describe: ${version_from_git}"

if [[ "${version_from_header}" != "${version_from_git}" ]]; then
    echo "esp_idf_version.h does not match 'git describe' output"
    exit 1
fi

if [[ "${version_from_cmake}" != "${version_from_git}" ]]; then
    echo "version.cmake does not match 'git describe' output"
    exit 1
fi
