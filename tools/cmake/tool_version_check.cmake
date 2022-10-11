function(check_expected_tool_version tool_name tool_path)
    # Function to check the tool used the expected version and warn otherwise
    set(tool_version_warning "Check Getting Started documentation or proceed at own risk.\n")
    set(tool_version_error "Check Getting Started documentation if the error continues.\n"
        "You can override this error and proceed with build by defining the IDF_MAINTAINER environment variable.\n")
    set(fixing_hint "Please try to run 'idf.py fullclean' to solve it.\n")

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)

    set(ENV{IDF_TOOLS_VERSION_HELPER} "1")
    # Use idf_tools.py to check if tool version is supported
    execute_process(
        COMMAND ${python} "${idf_path}/tools/idf_tools.py"
        "check-tool-supported" "--tool-name" "${tool_name}"
        "--exec-path" "${tool_path}"
        OUTPUT_VARIABLE is_version_supported
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    if(is_version_supported STREQUAL "False")
        # Version is not supported. Need to get supported versions list to print them to user
        execute_process(
            COMMAND ${python} "${idf_path}/tools/idf_tools.py"
            "get-tool-supported-versions" "--tool-name" "${tool_name}"
            OUTPUT_VARIABLE tool_supported_versions
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        # IDF maintainers can build projects with not supported versions with just a warning
        if($ENV{IDF_MAINTAINER})
            set(message_mode "WARNING")
        else()
            set(message_mode "FATAL_ERROR")
        endif()

        message(${message_mode} "\n"
                 "Tool doesn't match supported version from list "
                "${tool_supported_versions}: ${tool_path}\n"
                 ${fixing_hint})
    elseif(NOT is_version_supported STREQUAL "True")
        message(WARNING "Can not get version for tool: ${tool_path}\n" ${tool_version_warning})
    endif()
    unset(ENV{IDF_TOOLS_VERSION_HELPER})
endfunction()
