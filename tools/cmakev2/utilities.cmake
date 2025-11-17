# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# To maintain backward compatibility for those who might expect
# remove_duplicated_flags to be available when utilities.cmake is included,
# we ensure its presence. The remove_duplicated_flags function is primarily
# used by toolchain CMake files.
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/deduplicate_flags.cmake)

# Note: CMake does not support nested lists. The functions idf_die, idf_warn,
# idf_msg, and idf_dbg use ARGV# values because this is the only way to prevent
# arguments from being altered by CMake. ARGV and ARGN contain a flattened list
# of arguments, making it impossible to determine if any argument was
# originally a list.
#
# Using these functions has a side effect: the actual origin of the message
# appears as the first line of the backtrace.

#[[
.. cmakev2:function:: idf_die

    .. code-block:: cmake

        idf_die(<msg>...)

    *msg[in]*

        Message to print.

    Print error ``<msg>`` and abort the build process. Multiple messages are
    concatenated into a single message with no separator between them.

#]]
function(idf_die)
    set(joined "")
    math(EXPR last "${ARGC} - 1")
    foreach(i RANGE 0 ${last})
        string(APPEND joined "${ARGV${i}}")
    endforeach()
    message(FATAL_ERROR " IDF: ${joined}")
endfunction()

#[[
.. cmakev2:function:: idf_warn

    .. code-block:: cmake

        idf_warn(<msg>...)

    *msg[in]*

        Message to print.

    Print warning ``<msg>``.  Multiple messages are concatenated into a single
    message with no separator between them.
#]]
function(idf_warn)
    set(joined "")
    math(EXPR last "${ARGC} - 1")
    foreach(i RANGE 0 ${last})
        string(APPEND joined "${ARGV${i}}")
    endforeach()
    message(WARNING " IDF: ${joined}")
endfunction()

#[[
.. cmakev2:function:: idf_msg

    .. code-block:: cmake

        idf_msg(<msg>...)

    *msg[in]*

        Message to print.

    Print status ``<msg>``.  Multiple messages are concatenated into a single
    message with no separator between them.
#]]
function(idf_msg)
    set(joined "")
    math(EXPR last "${ARGC} - 1")
    foreach(i RANGE 0 ${last})
        string(APPEND joined "${ARGV${i}}")
    endforeach()
    message(STATUS " IDF: ${joined}")
endfunction()

#[[
.. cmakev2:function:: idf_dbg

    .. code-block:: cmake

        idf_dbg(<msg>...)

    *msg[in]*

        Message to print.

    Print debug ``<msg>``. Multiple messages are concatenated into a single
    message with no separator between them.
#]]
function(idf_dbg)
    set(joined "")
    math(EXPR last "${ARGC} - 1")
    foreach(i RANGE 0 ${last})
        string(APPEND joined "${ARGV${i}}")
    endforeach()
    message(DEBUG " IDF: ${joined}")
endfunction()

#[[
    idf_deprecated(<msg>...)

    *msg[in]*

        Message to print.

    Print deprecated ``<msg>``. Multiple messages are concatenated into a
    single message with no separator between them.
#]]
function(idf_deprecated)
    set(joined "")
    math(EXPR last "${ARGC} - 1")
    foreach(i RANGE 0 ${last})
        string(APPEND joined "${ARGV${i}}")
    endforeach()
    message(DEPRECATION " IDF: ${joined}")
endfunction()

#[[
    deprecate_variable(<var>)

    *var[in]*

        Deprecated variable name.

    Print a warning about the use of a deprecated variable.
#]]
function(deprecate_variable var)
    if(${var})
        idf_deprecated("The use of the '${var}' variable is deprecated and will be ignored.")
    endif()
endfunction()

#[[
    __get_real_target(TARGET <target>
                      OUTPUT <variable>)

    *TARGET[int]*

        Target name or target alias name.

    *OUTPUT[out]*

        Output variable to store the real target name.

    For a given ``<target>``, return the actual target if ``<target>`` is an
    alias. If ``<target>`` is the target itself, return it. If ``<target>`` is
    not a target at all, return ``NOTFOUND``.
#]]
function(__get_real_target)
    set(options)
    set(one_value TARGET OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    set(real_target NOTFOUND)
    if(TARGET "${ARG_TARGET}")
        get_target_property(aliased ${ARG_TARGET} ALIASED_TARGET)
        if(aliased)
            set(real_target ${aliased})
        else()
            set(real_target ${ARG_TARGET})
        endif()
    endif()
    set(${ARG_OUTPUT} "${real_target}" PARENT_SCOPE)
endfunction()

#[[
    __get_absolute_paths(PATHS <path>...
                         BASE_DIR <base_dir>
                         OUTPUT <variable>)

    *PATHS[in]*

        List of paths to convert to absolute paths.

    *BASE_DIR[in,opt]*

        Evaluate relative paths based on the specified ``<base_dir>``. If not
        provided, use CMAKE_CURRENT_SOURCE_DIR instead.

    *OUTPUT[out]*

        Output variable to store absolute paths.

    For a given ``PATHS``, return the absolute paths in ``OUTPUT``.
#]]
function(__get_absolute_paths)
    set(options)
    set(one_value BASE_DIR OUTPUT)
    set(multi_value PATHS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if(NOT DEFINED ARG_PATHS)
        set(ARG_PATHS "")
    endif()

    set(absolute_paths "")
    foreach(path IN LISTS ARG_PATHS)
        if(DEFINED ARG_BASE_DIR)
            get_filename_component(path_abs ${path} ABSOLUTE BASE_DIR "${ARG_BASE_DIR}")
        else()
            get_filename_component(path_abs ${path} ABSOLUTE)
        endif()
        list(APPEND absolute_paths "${path_abs}")
    endforeach()

    set(${ARG_OUTPUT} "${absolute_paths}" PARENT_SCOPE)
endfunction()

#[[
    __get_relative_paths(PATHS <path>...
                         BASE_DIR <base_dir>
                         OUTPUT <variable>)

    *PATHS[in]*

        List of paths to convert to relative paths.

    *BASE_DIR[in]*

        Base directory for finding relative paths.

    *OUTPUT[out]*

        Output variable to store relative paths.

    For a given ``PATHS``, return the relative paths to ``BASE_DIR`` in
    ``OUTPUT``.
#]]
function(__get_relative_paths)
    set(options)
    set(one_value BASE_DIR OUTPUT)
    set(multi_value PATHS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if(NOT DEFINED ARG_BASE_DIR)
        idf_die("BASE_DIR option is required")
    endif()

    if(NOT DEFINED ARG_PATHS)
        set(ARG_PATHS "")
    endif()

    set(relative_paths "")
    foreach(path IN LISTS ARG_PATHS)
        file(RELATIVE_PATH path_rel "${ARG_BASE_DIR}" "${path}")
        if(NOT path_rel)
            set(path_rel ".")
        endif()
        list(APPEND relative_paths "${path_rel}")
    endforeach()

    set(${ARG_OUTPUT} "${relative_paths}" PARENT_SCOPE)
endfunction()

#[[
    __get_default_value(VARIABLE <variable>
                        DEFAULT <value>)
                        OUTPUT <result>)

    *VARIABLE[in]*

        The name of the variable for which to obtain its default value.

    *DEFAULT[in]*

        Default variable value.

    *OUTPUT[out]*

        Variable to store the default value of ``<variable>``.

    Set the ``<result>`` to the value of ``<variable>`` based on the following
    order of precedence, with the first having the highest precedence.

    1. environmental variable
    2. CMake variable or value stored in CMake's cache
    3. ``<value>`` as specified with the ``DEFAULT`` option
#]]
function(__get_default_value)
    set(options)
    set(one_value VARIABLE DEFAULT OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_VARIABLE)
        idf_die("VARIABLE option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if(NOT DEFINED ARG_DEFAULT)
        set(ARG_DEFAULT "")
    endif()


    if(DEFINED ENV{${ARG_VARIABLE}})
        set(value "$ENV{${ARG_VARIABLE}}")
    elseif(DEFINED ${ARG_VARIABLE})
        set(value "${${ARG_VARIABLE}}")
    else()
        set(value "${ARG_DEFAULT}")
    endif()

    set(${ARG_OUTPUT} "${value}" PARENT_SCOPE)
endfunction()

#[[
   __get_sdkconfig_option(OPTION <option>
                          SDKCONFIG <path>)
                          OUTPUT <variable>)

    *OPTION[in]*

        Option name in sdkconfig to obtain the value of.

    *SDKCONFIG[in]*

        Sdkconfig file path.

    *OUTPUT[out]*

        Variable to store the value of sdkconfig ``<option>``.

    Search for the ``<option>`` in the sdkconfig file specified by ``<path>``
    and store its value in ``<variable>``. If the option is not found, return
    ``NOTFOUND``.
#]]
function(__get_sdkconfig_option)
    set(options)
    set(one_value OPTION SDKCONFIG OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OPTION)
        idf_die("OPTION option is required")
    endif()

    if(NOT DEFINED ARG_SDKCONFIG)
        idf_die("SDKCONFIG option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    set(value NOTFOUND)

    if(NOT EXISTS "${ARG_SDKCONFIG}")
        set(${ARG_OUTPUT} "${value}" PARENT_SCOPE)
        return()
    endif()

    file(STRINGS "${ARG_SDKCONFIG}" lines)
    foreach(line ${lines})
        if(NOT "${line}" MATCHES "^${ARG_OPTION}=\"[^\"]+\"$")
            continue()
        endif()

        string(REGEX REPLACE "${ARG_OPTION}=\"([^\"]+)\"" "\\1" value "${line}")
        break()
    endforeach()

    set(${ARG_OUTPUT} "${value}" PARENT_SCOPE)
endfunction()

#[[
    __set_property(TARGET <target>
                   PROPERTY <property>)
                   PROPERTIES <properties>
                   VALUE <value>
                   APPEND)

    *TARGET[in]*

        The target name to attach the property.

    *PROPERTY[in]*

        Property name.

    *PROPERTIES[in]*

        The target property containing list of properties.

    *VALUE[in]*

        Property value.

    *APPEND*

        Append the value to the property's current value instead of replacing
        it.

    Set the ``PROPERTY`` to ``VALUE`` for the ``TARGET``, and also record the
    ``PROPERTY`` name in the list of all properties for the ``TARGET``. The
    property name where all ``TARGET`` property names are stored is specified
    by the ``PROPERTIES`` option. If ``APPEND`` is specified, the property will
    be added rather than set.

    Note: The cmake_parse_arguments function does not preserve empty arguments,
    as explained in CMP0174. We do not verify if VALUE is defined because it
    might have been passed as an empty string or list.

    Note: The VALUE is treated as a multi-value because if a list is provided
    as the value, CMake assigns only the first entry to the VALUE if parsed as
    one-value. The remaining entries are stored in <prefix>_UNPARSED_ARGUMENTS.
#]]
function(__set_property)
    set(options APPEND)
    set(one_value TARGET PROPERTY PROPERTIES)
    set(multi_value VALUE)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_PROPERTY)
        idf_die("PROPERTY option is required")
    endif()

    if(NOT DEFINED ARG_PROPERTIES)
        idf_die("PROPERTIES option is required")
    endif()

    get_property(properties TARGET ${ARG_TARGET} PROPERTY ${ARG_PROPERTIES})
    if(NOT ARG_PROPERTY IN_LIST properties)
        list(APPEND properties "${ARG_PROPERTY}")
        set_property(TARGET ${ARG_TARGET} PROPERTY ${ARG_PROPERTIES} "${properties}")
    endif()
    if(ARG_APPEND)
        set_property(TARGET ${ARG_TARGET} APPEND PROPERTY ${ARG_PROPERTY} "${ARG_VALUE}")
    else()
        set_property(TARGET ${ARG_TARGET} PROPERTY ${ARG_PROPERTY} "${ARG_VALUE}")
    endif()
endfunction()

#[[
    __get_property(TARGET <target>
                   PROPERTY <property>)
                   OUTPUT <variable>
                   GENERATOR_EXPRESSION)

    *TARGET[in]*

        The target from which to obtain the property value.

    *PROPERTY[in]*

        Property name.

    *OUTPUT[out]*

        Output variable to store the property value.

    *GENERATOR_EXPRESSION*

        Obtain the generator expression for the property rather than the actual
        value.

    Get the value of the specified ``PROPERTY`` from ``TARGET`` and store it in
    the ``OUTPUT`` variable.
#]]
function(__get_property)
    set(options GENERATOR_EXPRESSION)
    set(one_value TARGET PROPERTY OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_PROPERTY)
        idf_die("PROPERTY option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("PROPERTIES option is required")
    endif()

    if(ARG_GENERATOR_EXPRESSION)
        set(value "$<TARGET_PROPERTY:${ARG_TARGET},${ARG_PROPERTY}>")
    else()
        get_property(value TARGET ${ARG_TARGET} PROPERTY ${ARG_PROPERTY})
    endif()

    set(${ARG_OUTPUT} ${value} PARENT_SCOPE)
endfunction()

#[[
    __dump_all_properties()

    Dump all properties, including build properties, component properties, and
    library properties.
#]]
function(__dump_all_properties)
    __dump_build_properties()
    idf_build_get_property(component_names COMPONENTS_DISCOVERED)
    __dump_component_properties("${component_names}")
    idf_build_get_property(library_interfaces LIBRARY_INTERFACES)
    __dump_library_properties("${library_interfaces}")
endfunction()

#[[
    __split(STRING <string>
            OUTPUT <variable>
            [SEPARATOR <sep>]
            [REMOVE_EMPTY]
            [STRIP])

    *STRING[in]*

        Input string to be split.

    *OUTPUT[out]*

        Output variable where the split strings will be stored.

    *SEPARATOR[in,opt]*

        The separator string used to split the string.  By default split the
        string using newlines: "\r\n", "\n", and "\r".

    *REMOVE_EMPTY[opt]*

        Remove empty split strings.

    *STRIP[opt]*

        Remove leading and trailing spaces from split strings.

    Split the ``STRING`` using the ``SEPARATOR`` and store the resulting list
    of split strings in ``OUTPUT``. If ``SEPARATOR`` is not provided, the split
    will be based on newlines. When ``STRIP`` is specified, leading and
    trailing whitespaces will be removed. If ``REMOVE_EMPTY`` is specified,
    empty strings will be removed from the ``OUTPUT`` list.
#]]
function(__split)
    set(options STRIP REMOVE_EMPTY)
    set(one_value STRING SEPARATOR OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_STRING)
        idf_die("STRING option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if(NOT DEFINED ARG_SEPARATOR)
        string(REPLACE "\r\n" "\n" ARG_STRING "${ARG_STRING}")
        string(REPLACE "\r" "\n" ARG_STRING "${ARG_STRING}")
        set(ARG_SEPARATOR "\n")
    endif()

    string(REPLACE "${ARG_SEPARATOR}" ";" lines "${ARG_STRING}")

    if(NOT DEFINED ARG_STRIP AND NOT DEFINED ARG_REMOVE_EMPTY)
        set(${ARG_OUTPUT} "${lines}" PARENT_SCOPE)
        return()
    endif()

    set(filtered_lines "")
    foreach(line IN LISTS lines)
        if(ARG_STRIP)
            string(STRIP "${line}" line)
        endif()
        if(ARG_REMOVE_EMPTY AND "${line}" STREQUAL "")
            continue()
        endif()
        list(APPEND filtered_lines "${line}")
    endforeach()
    set(${ARG_OUTPUT} "${filtered_lines}" PARENT_SCOPE)
endfunction()

#[[
    __get_compile_options(OUTPUT <variable>)

    *OUTPUT[out]*

        List of generator expressions for C, CXX, and ASM compile options

    Gather the compilation options from COMPILE_OPTIONS, C_COMPILE_OPTIONS,
    CXX_COMPILE_OPTIONS, and ASM_COMPILE_OPTIONS build properties into a single
    list using generator expressions. This list can then be used with the
    target_compile_options call.
#]]
function(__get_compile_options)
    set(options)
    set(one_value OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()
    idf_build_get_property(compile_options COMPILE_OPTIONS GENERATOR_EXPRESSION)
    idf_build_get_property(c_compile_options C_COMPILE_OPTIONS GENERATOR_EXPRESSION)
    idf_build_get_property(cxx_compile_options CXX_COMPILE_OPTIONS GENERATOR_EXPRESSION)
    idf_build_get_property(asm_compile_options ASM_COMPILE_OPTIONS GENERATOR_EXPRESSION)

    foreach(option IN LISTS c_compile_options)
        list(APPEND compile_options $<$<COMPILE_LANGUAGE:C>:${option}>)
    endforeach()
    foreach(option IN LISTS cxx_compile_options)
        list(APPEND compile_options $<$<COMPILE_LANGUAGE:CXX>:${option}>)
    endforeach()
    foreach(option IN LISTS asm_compile_options)
        list(APPEND compile_options $<$<COMPILE_LANGUAGE:ASM>:${option}>)
    endforeach()
    set(${ARG_OUTPUT} "${compile_options}" PARENT_SCOPE)
endfunction()

#[[
    spaces2list(<variable>)

    *variable[in,out]*

        String with space-delimited values.

    Take a variable containing space-delimited values and convert it into a
    CMake list with semicolon-delimited values.

    Note: do not use this for directories or full paths, as they may contain
    spaces.

    TODO: look at cmake separate_arguments, which is quote-aware
#]]
function(spaces2list variable_name)
    string(REPLACE " " ";" tmp "${${variable_name}}")
    set("${variable_name}" "${tmp}" PARENT_SCOPE)
endfunction()

#[[
    add_prefix(<variable> <prefix> <item>...)

    *variable[out]*

        Output variable with list of prefixed items.

    *prefix[in]*

        Prefix string.

    *item[in]*

        Items to be prefixed.

    Add a prefix to each item in the given list.
#]]
function(add_prefix var prefix)
    foreach(elm ${ARGN})
        list(APPEND newlist "${prefix}${elm}")
    endforeach()
    set(${var} "${newlist}" PARENT_SCOPE)
endfunction()

#[[
    fail_target(<target> <line0> [<line>...])

    *target[in]*

        Fail target name.

    *line0[in]*

        First line of the failed message.

    *line[in,opt]*

        Optional additional message lines.

    Create a phony target which fails when invoked. This is used when the
    necessary conditions for a target are not met, such as configuration.
    Rather than omitting the target altogether, we fail execution with a
    helpful message.
#]]
function(fail_target target_name message_line0)
    idf_build_get_property(idf_path IDF_PATH)
    set(message_lines COMMAND ${CMAKE_COMMAND} -E echo "${message_line0}")
    foreach(message_line ${ARGN})
        set(message_lines ${message_lines} COMMAND ${CMAKE_COMMAND} -E echo "${message_line}")
    endforeach()
    # Generate a timestamp file that gets included. When deleted on build, this forces CMake
    # to rerun.
    set(fail_message "Failed executing target (see errors on lines above)")
    add_custom_target(${target_name}
        ${message_lines}
        COMMAND ${CMAKE_COMMAND} -E env FAIL_MESSAGE=${fail_message}
                ${CMAKE_COMMAND} -P ${idf_path}/tools/cmake/scripts/fail.cmake
        VERBATIM)
endfunction()

#[[
    file_generate(<output>
                  [INPUT <file>]
                  [CONTENT <string>])

    *output[in]*

        Output file name.

    *INPUT[in]*

        Input file name.

    *CONTENT[in]*

        Input string.

    Utility to generate file and have the output automatically added to cleaned
    files.
#]]
function(file_generate output)
    cmake_parse_arguments(_ "" "INPUT;CONTENT" "" ${ARGN})

    if(__INPUT)
        file(GENERATE OUTPUT "${output}" INPUT "${__INPUT}")
    elseif(__CONTENT)
        file(GENERATE OUTPUT "${output}" CONTENT "${__CONTENT}")
    else()
        message(FATAL_ERROR "Content to generate not specified.")
    endif()

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${output}")
endfunction()

#[[
    add_deprecated_target_alias(<old_target> <new_target>)

    *old_target[in]*

        Old target name.

    *new_target[in]*

        New target name.

    Create an alias for the existing target and display a deprecation warning.
#]]
function(add_deprecated_target_alias old_target new_target)
    add_custom_target(${old_target}
     # `COMMAND` is important to print the `COMMENT` message at the end of the target action.
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMENT "Warning: command \"${old_target}\" is deprecated. Have you wanted to run \"${new_target}\" instead?"
    )
    add_dependencies(${old_target} ${new_target})
endfunction()

#[[
    target_add_binary_data(<target> <embed_file> <embed_type>
                           [RENAME_TO <symbol>])
                           [DEPENDS <dependency>...])

    *target[in]*

        Attach the source file with embedded data to the specified target.

    *embed_file[in]*

        File containing the embedded data.

    *embed_type[in]*

        BINARY or TEXT

    *RENAME_TO[in,opt]*

        Use the given symbol name for the embedded data. If no symbol name is
        provided, the embed_file file name will be used instead.

    *DEPENDS[in,opt]*

        List of additional dependencies for the generated file containing
        embedded data.

    Add binary data into the build target by converting it into a generated
    source file, which is then compiled into a binary object as part of the
    build process.
#]]
function(target_add_binary_data target embed_file embed_type)
    cmake_parse_arguments(_ "" "RENAME_TO" "DEPENDS" ${ARGN})
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(idf_path IDF_PATH)

    get_filename_component(embed_file "${embed_file}" ABSOLUTE)

    get_filename_component(name "${embed_file}" NAME)
    set(embed_srcfile "${build_dir}/${name}.S")

    set(rename_to_arg)
    if(__RENAME_TO)  # use a predefined variable name
        set(rename_to_arg -D "VARIABLE_BASENAME=${__RENAME_TO}")
    endif()

    add_custom_command(OUTPUT "${embed_srcfile}"
        COMMAND "${CMAKE_COMMAND}"
        -D "DATA_FILE=${embed_file}"
        -D "SOURCE_FILE=${embed_srcfile}"
        ${rename_to_arg}
        -D "FILE_TYPE=${embed_type}"
        -P "${idf_path}/tools/cmake/scripts/data_file_embed_asm.cmake"
        MAIN_DEPENDENCY "${embed_file}"
        DEPENDS "${idf_path}/tools/cmake/scripts/data_file_embed_asm.cmake" ${__DEPENDS}
        WORKING_DIRECTORY "${build_dir}"
        VERBATIM)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${embed_srcfile}")

    target_sources("${target}" PRIVATE "${embed_srcfile}")
endfunction()

#[[
    add_prebuilt_library(<target> <lib>
                         [REQUIRES <component>...])
                         [PRIV_REQUIRES <component>...])

    *target[in]*

        Target name for the imported library.

    *library[in]*

        Imported library path.

    *REQUIRES[in,opt]*

        Optional dependency on other components.

    *PRIV_REQUIRES[in,opt]*

        Optional private dependency on other components.

    Add prebuilt library with support for adding dependencies on ESP-IDF
    components.
#]]
function(add_prebuilt_library target_name lib_path)
    cmake_parse_arguments(_ "" "" "REQUIRES;PRIV_REQUIRES" ${ARGN})

    get_filename_component(lib_path "${lib_path}"
                ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

    add_library(${target_name} STATIC IMPORTED GLOBAL)
    set_property(TARGET ${target_name} PROPERTY IMPORTED_LOCATION ${lib_path})

    foreach(req ${__REQUIRES})
        idf_component_get_property(req_lib "${req}" COMPONENT_LIB)
        set_property(TARGET ${target_name} APPEND PROPERTY LINK_LIBRARIES "${req_lib}")
        set_property(TARGET ${target_name} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${req_lib}")
    endforeach()

    foreach(req ${__PRIV_REQUIRES})
        idf_component_get_property(req_lib "${req}" COMPONENT_LIB)
        set_property(TARGET ${target_name} APPEND PROPERTY LINK_LIBRARIES "${req_lib}")
        set_property(TARGET ${target_name} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "$<LINK_ONLY:${req_lib}>")
    endforeach()
endfunction()

#[[
    __get_target_dependencies(TARGET <target>
                              OUTPUT <variable>)

    *TARGET[in]*

        Target for which to get dependencies.

    *OUTPUT[out]*

        Output variable where the list of dependencies will be stored.

    Recursively retrieve the list of dependencies for ``TARGET``.
#]]
function(__get_target_dependencies)
    set(options)
    set(one_value TARGET OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    set(targets_list "${${ARG_OUTPUT}}")
    set(target_deps)

    get_target_property(target_deps_link ${ARG_TARGET} LINK_LIBRARIES)
    if(target_deps_link)
        list(APPEND target_deps "${target_deps_link}")
    endif()
    get_target_property(target_deps_interface ${ARG_TARGET} INTERFACE_LINK_LIBRARIES)
    if(target_deps_interface)
        list(APPEND target_deps "${target_deps_interface}")
    endif()
    list(REMOVE_DUPLICATES target_deps)

    if(NOT target_deps)
        # This target has no dependencies, nothing to do.
        return()
    endif()

    foreach(dep IN LISTS target_deps)
        if(dep IN_LIST targets_list)
            # This dependency has already been processed, skip it.
            continue()
        endif()

        if(dep MATCHES "^\\$<.*>$")
            # Skip generator expressions
            continue()
        endif()

        if(NOT TARGET ${dep})
            # LINK_LIBRARIES may contain various non-library-related linker flags
            # (-u, -L, -l, etc.), skip them.
            if(dep MATCHES "^-" OR dep MATCHES "^\:\:")
                continue()
            endif()

            # If the dependency is not a target, it may be a library. Add it to the list.
            list(APPEND targets_list ${dep})
        else()
            # Recursively add the dependencies of this target.
            list(APPEND targets_list ${dep})
            __get_target_dependencies(TARGET ${dep} OUTPUT targets_list)
        endif()
    endforeach()

    set(${ARG_OUTPUT} ${targets_list} PARENT_SCOPE)
endfunction()

#[[
    __get_executable_library_or_die(TARGET <executable>
                                    OUTPUT <library>)

    *TARGET[in]*

        Executable target.

    *OUTPUT[out]*

        Output variable to store the library interface target linked to the
        executable.

    Search for the library interface target in the LIBRARY_INTERFACE executable
    property. If not found, search for the library interface target created with
    the idf_build_library() function and linked to the executable. Examine the
    LINK_LIBRARIES for the executable and the LIBRARY_INTERFACES build property,
    which stores all library interface targets created by the idf_build_library()
    function.
#]]
function(__get_executable_library_or_die)
    set(options)
    set(one_value TARGET OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    get_target_property(type "${ARG_TARGET}" TYPE)
    if(NOT type STREQUAL "EXECUTABLE")
        idf_die("The type of target '${ARG_TARGET}' is '${type}', "
                "but an 'EXECUTABLE' target type is expected.")
    endif()

    idf_build_get_property(libraries LIBRARY_INTERFACES)
    get_target_property(library "${ARG_TARGET}" LIBRARY_INTERFACE)
    if(NOT "${library}" IN_LIST libraries)
        # The library interface is not stored in the LIBRARY_INTERFACE
        # executable property, so the executable was not created by the
        # idf_build_executable function. Try searching for the library
        # interface in the LINK_LIBRARIES property.
        set(library NOTFOUND)
        get_target_property(targets "${ARG_TARGET}" LINK_LIBRARIES)
        foreach(target IN LISTS targets)
            if("${target}" IN_LIST libraries)
                set(library "${target}")
                break()
            endif()
        endforeach()
    endif()

    if("${library}" STREQUAL "NOTFOUND")
        idf_die("No library interface target linked to the '${ARG_TARGET}' executable")
    endif()
    set(${ARG_OUTPUT} "${library}" PARENT_SCOPE)
endfunction()

#[[
    __make_json_list(<list> OUTPUT <variable>)

    *list[in]*

        CMake string containing a list to be converted into JSON format.

    *OUTPUT[out]*

        Output variable name to store the JSON list.

    Convert a CMake list to a JSON list and store it in a OUTPUT variable.
#]]
function(__make_json_list input)
    set(options)
    set(one_value OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    list(LENGTH input length)
    if(${length})
        string(REPLACE ";" "\", \"" result "[ \"${input}\" ]")
    else()
        set(result "[]")
    endif()
    set("${ARG_OUTPUT}" "${result}" PARENT_SCOPE)
endfunction()

#[[
    fail_at_build_time(<target> <message>...)

    *target[in]*

        Target name to create and add to ALL.

    *message[in]*

        Error message to display. Multiple messages may be provided.

    Create a phony target that intentionally fails the build, displaying the
    error messages specified in the message arguments. An empty CMake file is
    created and included, which is then deleted when the target is executed.
    This forces CMake to rerun, as the file is added to the CMake rerun
    dependencies.
#]]
function(fail_at_build_time target_name message_line0)
    idf_build_get_property(idf_path IDF_PATH)
    set(message_lines COMMAND ${CMAKE_COMMAND} -E echo "${message_line0}")
    foreach(message_line ${ARGN})
        set(message_lines ${message_lines} COMMAND ${CMAKE_COMMAND} -E echo "${message_line}")
    endforeach()
    # Generate a timestamp file that gets included. When deleted on build, this forces CMake
    # to rerun.
    string(RANDOM filename)
    set(filename "${CMAKE_CURRENT_BINARY_DIR}/${filename}.cmake")
    file(WRITE "${filename}" "")
    include("${filename}")
    set(fail_message "Failing the build (see errors on lines above)")
    add_custom_target(${target_name} ALL
        ${message_lines}
        COMMAND ${CMAKE_COMMAND} -E remove "${filename}"
        COMMAND ${CMAKE_COMMAND} -E env FAIL_MESSAGE=${fail_message}
                ${CMAKE_COMMAND} -P ${idf_path}/tools/cmake/scripts/fail.cmake
        VERBATIM)
endfunction()

#[[
    __preprocess_linker_script(<script_in> <script_out>)

    *script_in[in]*

        Path to the linker script that needs preprocessing.

    *script_out[out]*

        Path where the preprocessed linker script will be saved.

    Run the C preprocessor on ``script_in`` and store the result in
    ``script_out``.
#]]
function(__preprocess_linker_script script_in script_out)
    idf_build_get_property(sdkconfig_header __SDKCONFIG_HEADER)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(config_dir CONFIG_DIR)
    idf_build_get_property(idf_target IDF_TARGET)

    # This approach is not ideal, but it is the current method used in cmakev1
    # for the esp_system component. The linker script files for specific
    # targets within the esp_system component include the ld.common file. This
    # adds the ld.common directory to the search path for the C preprocessor.
    # This method works for the specific directory layout of esp_common, but if
    # other components with different layouts adopt this approach, adjustments
    # will be necessary. It might be better to include the files using relative
    # paths in the linker scripts.
    get_filename_component(script_parent_dir "${script_in}" DIRECTORY)
    get_filename_component(script_parent_name "${script_parent_dir}" NAME)
    set(extra_cflags "")
    if(script_parent_name STREQUAL idf_target)
        get_filename_component(dir_to_include "${script_parent_dir}" DIRECTORY)
        set(extra_cflags "-I\"${dir_to_include}\"")
    endif()

    set(linker_script_generator "${idf_path}/tools/cmake/linker_script_preprocessor.cmake")

    add_custom_command(
        OUTPUT ${script_out}
        COMMAND ${CMAKE_COMMAND}
            "-DCC=${CMAKE_C_COMPILER}"
            "-DSOURCE=${script_in}"
            "-DTARGET=${script_out}"
            "-DCFLAGS=-I\"${config_dir}\" ${extra_cflags}"
            -P "${linker_script_generator}"
        MAIN_DEPENDENCY "${script_in}"
        DEPENDS "${sdkconfig_header}"
        COMMENT "Preprocessing linker script ${script_in} -> ${script_out}"
        VERBATIM)
endfunction()

#[[
    __remove_genex(<list>)

    *list[in,out]*

        List from which entries containing generator expressions should be
        removed.

    Remove entries containing generator expressions from the ``list``. The
    ``list`` is modified in place.
#]]
function(__remove_genex list)
    set(result "")
    foreach(item IN LISTS ${list})
        # If the item contains a $<...> pattern, skip it
        string(REGEX MATCH "\\$<[^>]+>" match "${item}")
        if(NOT match)
            # Only append if no generator expression found
            list(APPEND result "${item}")
        endif()
    endforeach()
    set(${list} "${result}" PARENT_SCOPE)
endfunction()

#[[
    __list_intersection(<list1> <list2> <output_var>)

    *list1[in]*

        First list.

    *list2[in]*

        Second list.

    *output_var[out]*

        Variable name where the resulting list will be stored.

    Return the intersection of elements from ``list1`` and ``list2``. The
    resulting list is stored in the ``output_var`` list.
#]]
function(__list_intersection list1 list2 output_var)
    set(result "")
    foreach(item IN LISTS ${list1})
        if(item IN_LIST ${list2})
            list(APPEND result "${item}")
        endif()
    endforeach()
    set(${output_var} "${result}" PARENT_SCOPE)
endfunction()

#[[
    __list_difference(<list1> <list2> <output_var>)

    *list1[in]*

        First list.

    *list2[in]*

        Second list.

    *output_var[out]*

        Variable name where the resulting list will be stored.

    Return the difference between the elements of ``list1`` and ``list2``. In
    other words, the elements that are present in ``list1`` but not in
    ``list2``.  The resulting list is stored in the ``output_var`` list.
#]]
function(__list_difference list1 list2 output_var)
    set(result "")
    foreach(item IN LISTS ${list1})
        if(NOT item IN_LIST ${list2})
            list(APPEND result "${item}")
        endif()
    endforeach()
    set(${output_var} "${result}" PARENT_SCOPE)
endfunction()
