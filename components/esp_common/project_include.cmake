#
# Warn if the toolchain version doesn't match
#
if(NOT (${target} STREQUAL "linux" OR CMAKE_C_COMPILER_ID MATCHES "Clang"))
    get_expected_ctng_version(expected_toolchain expected_gcc)
    gcc_version_check("${expected_gcc}")
    crosstool_version_check("${expected_toolchain}")
endif()

if(NOT ${target} STREQUAL "linux" AND CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(WARNING "Building ESP-IDF with clang is an experimental feature and is not yet officially supported.")
endif()
