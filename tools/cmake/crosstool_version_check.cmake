# Function to check the toolchain used the expected version
# of crosstool, and warn otherwise

set(ctng_version_warning "Check Getting Started documentation or proceed at own risk.")

function(gcc_version_check expected_gcc_version)
    if(NOT "${CMAKE_C_COMPILER_VERSION}" STREQUAL "${expected_gcc_version}")
        message(WARNING "Xtensa toolchain ${CMAKE_C_COMPILER} version ${CMAKE_C_COMPILER_VERSION} "
            "is not the supported version ${expected_gcc_version}. ${ctng_version_warning}")
    endif()
endfunction()

function(crosstool_version_check expected_ctng_version)
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -v
        ERROR_VARIABLE toolchain_stderr
        OUTPUT_QUIET)

    string(REGEX MATCH "crosstool-ng-[0-9a-g\\.-]+" ctng_version "${toolchain_stderr}")
    string(REPLACE "crosstool-ng-" "" ctng_version "${ctng_version}")
    # We use FIND to match version instead of STREQUAL because some toolchains are built
    # with longer git hash strings than others. This will match any version which starts with
    # the expected version string.
    string(FIND "${ctng_version}" "${expected_ctng_version}" found_expected_version)
    if(NOT ctng_version)
        message(WARNING "Xtensa toolchain ${CMAKE_C_COMPILER} does not appear to be built with crosstool-ng. "
            "${ctng_version_warning}")
    elseif(found_expected_version EQUAL -1)
        message(WARNING "Xtensa toolchain ${CMAKE_C_COMPILER} crosstool-ng version ${ctng_version} "
            "doesn't match supported version ${expected_ctng_version}. ${ctng_version_warning}")
    endif()
endfunction()
