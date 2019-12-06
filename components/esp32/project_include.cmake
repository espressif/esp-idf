if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    # We do this here as well as in CMakeLists.txt, because targets that
    # are not part of the ESP-IDF build system (for cases where a generic
    # non-IDF CMakeLists.txt file is imported into a component) don't depend
    # on the esp32 component so don't get the extra flag. This handles that case.
    idf_build_set_property(COMPILE_OPTIONS "-mfix-esp32-psram-cache-issue" APPEND)
endif()

# Check toolchain is configured properly in cmake
if(NOT ( ${CMAKE_SYSTEM_NAME} STREQUAL "Generic" AND ${CMAKE_C_COMPILER} MATCHES xtensa))
    message(FATAL_ERROR "Internal error, toolchain has not been set correctly by project "
        "(or an invalid CMakeCache.txt file has been generated somehow)")
endif()

#
# Warn if the toolchain version doesn't match
#
# TODO: make these platform-specific for diff toolchains
get_expected_ctng_version(expected_toolchain expected_gcc)
gcc_version_check("${expected_gcc}")
crosstool_version_check("${expected_toolchain}")