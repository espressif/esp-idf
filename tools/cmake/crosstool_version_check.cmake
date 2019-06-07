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
        COMMAND ${CMAKE_C_COMPILER} --version
        OUTPUT_VARIABLE toolchain_version
        ERROR_QUIET)

    string(REGEX REPLACE ".*(crosstool-NG ([^\)]+)).*\n" "\\2" ctng_version "${toolchain_version}")
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

function(get_expected_ctng_version _toolchain_ver _gcc_ver)
    idf_build_get_property(idf_path IDF_PATH)
    file(STRINGS ${idf_path}/tools/toolchain_versions.mk config_contents)
    foreach(name_and_value ${config_contents})
        # Strip spaces
        string(REPLACE " " "" name_and_value ${name_and_value})
        # Find variable name
        string(REGEX MATCH "^[^=]+" name ${name_and_value})
        # Find the value
        string(REPLACE "${name}=" "" value ${name_and_value})
        # Getting values
        if("${name}" STREQUAL "SUPPORTED_TOOLCHAIN_COMMIT_DESC")
            set("${_toolchain_ver}" "${value}" PARENT_SCOPE)
        elseif("${name}" STREQUAL "SUPPORTED_TOOLCHAIN_GCC_VERSIONS")
            set(${_gcc_ver} "${value}" PARENT_SCOPE)
        endif()
    endforeach()
endfunction()
