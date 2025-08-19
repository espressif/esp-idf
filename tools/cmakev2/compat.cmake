# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

#[[
   check_expected_tool_version()

   Function to verify if the tool is using the expected version and issue a
   warning if it is not.
#]]
function(check_expected_tool_version tool_name tool_path)
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

        idf_msg(${message_mode} "\n"
                 "Tool doesn't match supported version from list "
                "${tool_supported_versions}: ${tool_path}\n"
                 ${fixing_hint})
    elseif(NOT is_version_supported STREQUAL "True")
        idf_warn("Can not get version for tool: ${tool_path}\n" ${tool_version_warning})
    endif()
    unset(ENV{IDF_TOOLS_VERSION_HELPER})
endfunction()

#[[
   __get_component_sources([SRCS <src>...]
                           [SRC_DIRS <dir>...]
                           [EXCLUDE_SRCS <exclude>...]
                           OUTPUT <var>)

   :SRCS[in,opt]: Optional list of sources.
   :SRC_DIRS[in,opt]: Option list of source directories.
   :EXCLUDE_SRCS[in,opt]: Optional list of sources to exclude.
   :OUTPUT[out]: Output variable to store the list of component sources.

   This helper function gathers component sources from the arguments specified
   in either ``SRCS`` or ``SRC_DIRS``. If both are provided, the ``SRC_DIRS`` option
   is disregarded. The sources are collected as absolute paths relative to the
   component's directory. If ``EXCLUDE_SRCS`` is specified, the source files
   listed in it are excluded. The final list of component sources is returned
   in the ``OUTPUT`` variable.

   The ``COMPONENT_NAME`` and ``COMPONENT_DIR`` variables are provided by the
   build system when ``idf_component_register`` is called.
#]]
function(__get_component_sources)
    set(options)
    set(one_value OUTPUT)
    set(multi_value SRCS SRC_DIRS EXCLUDE_SRCS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if(DEFINED ARG_SRCS AND DEFINED ARG_SRC_DIRS)
        idf_warn("SRCS and SRC_DIRS are both specified for '${COMPONENT_NAME}' "
                 "in '${COMPONENT_DIR}'. Ignoring SRC_DIRS.")
        unset(ARG_SRC_DIRS)
    endif()

    set(sources)

    if(ARG_SRCS)
        __get_absolute_paths(PATHS "${ARG_SRCS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT sources)
    elseif(ARG_SRC_DIRS)
        __get_absolute_paths(PATHS "${ARG_SRC_DIRS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT dirs)
        foreach(dir IN LISTS dirs)
            if(NOT IS_DIRECTORY "${dir}")
                idf_die("SRC_DIRS entry '${dir}' does not exist for component "
                        "'${COMPONENT_NAME}' in '${COMPONENT_DIR}'.")
            endif()

            file(GLOB dir_sources "${dir}/*.c" "${dir}/*.cpp" "${dir}/*.S")
            list(SORT dir_sources)

            if(NOT dir_sources)
                idf_warn("No source files found for SRC_DIRS entry '${dir}' for "
                         "'${COMPONENT_NAME}' in '${COMPONENT_DIR}'.")
                continue()
            endif()

            list(APPEND sources "${dir_sources}")
        endforeach()
    endif()

    if(ARG_EXCLUDE_SRCS)
        __get_absolute_paths(PATHS "${ARG_EXCLUDE_SRCS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT exclude_srcs)
        foreach(src IN LISTS exclude_srcs)
            list(REMOVE_ITEM sources "${src}")
        endforeach()
    endif()

    list(REMOVE_DUPLICATES sources)
    set(${ARG_OUTPUT} "${sources}" PARENT_SCOPE)
endfunction()

#[[api
.. cmakev2:function:: target_linker_script

   .. code-block:: cmake

      target_linker_script(<target> <deptype> <scriptfile>...)

#]]
function(target_linker_script target deptype scriptfiles)
    # FIXME: This is just a placeholder without implementation.
endfunction()
