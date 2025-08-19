# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Note: CMake does not support nested lists. The functions idf_die, idf_warn,
# idf_msg, and idf_dbg use ARGV# values because this is the only way to prevent
# arguments from being altered by CMake. ARGV and ARGN contain a flattened list
# of arguments, making it impossible to determine if any argument was
# originally a list.
#
# Using these functions has a side effect: the actual origin of the message
# appears as the first line of the backtrace.

#[[api
.. cmakev2:function:: idf_die

   .. code-block:: cmake

      idf_die(<msg>...)

   :msg[in]: Message to print.

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

#[[api
.. cmakev2:function:: idf_warn

   .. code-block:: cmake

      idf_warn(<msg>...)

   :msg[in]: Message to print.

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

#[[api
.. cmakev2:function:: idf_msg

   .. code-block:: cmake

      idf_msg(<msg>...)

   :msg[in]: Message to print.

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

#[[api
.. cmakev2:function:: idf_dbg

   .. code-block:: cmake

      idf_dbg(<msg>...)

   :msg[in]: Message to print.

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
   __get_real_target(TARGET <target>
                     OUTPUT <variable>)

   :TARGET[int]: Target name or target alias name.
   :OUTPUT[out]: Output variable to store the real target name.

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

   :PATHS[in]: List of paths to convert to absolute paths.
   :BASE_DIR[in,opt]: Evaluate relative paths based on the specified
                      ``<base_dir>``. If not provided, use CMAKE_CURRENT_SOURCE_DIR
                      instead.
   :OUTPUT[out]: Output variable to store absolute paths.

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
   __get_default_value(VARIABLE <variable>
                       DEFAULT <value>)
                       OUTPUT <result>)

   :VARIABLE[in]: The name of the variable for which to obtain its default
                  value.
   :DEFAULT[in]: Default variable value.
   :OUTPUT[out]: Variable to store the default value of ``<variable>``.

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
   __get_config_option(OPTION <option>
                       SDKCONFIG <path>)
                       OUTPUT <variable>)

   :OPTION[in]: Option name in sdkconfig to obtain the value of.
   :SDKCONFIG[in]: Sdkconfig file path.
   :OUTPUT[out]: Variable to store the value of sdkconfig ``<option>``.

   Search for the ``<option>`` in the sdkconfig file specified by ``<path>`` and store
   its value in ``<variable>``. If the option is not found, return ``NOTFOUND``.
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

   :TARGET[in]: The target name to attach the property.
   :PROPERTY[in]: Property name.
   :PROPERTIES[in]: The target property containing list of properties.
   :VALUE[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

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

   :TARGET[in]: The target from which to obtain the property value.
   :PROPERTY[in]: Property name.
   :OUTPUT[out]: Output variable to store the property value.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

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

   :STRING[in]: Input string to be split.
   :OUTPUT[out]: Output variable where the split strings will be stored.
   :SEPARATOR[in,opt]: The separator string used to split the string.
                       By default split the string using newlines:
                       "\r\n", "\n", and "\r".
   :REMOVE_EMPTY[opt]: Remove empty split strings.
   :STRIP[opt]: Remove leading and trailing spaces from split strings.

   Split the ``STRING`` using the ``SEPARATOR`` and store the resulting list of
   split strings in ``OUTPUT``. If ``SEPARATOR`` is not provided, the split
   will be based on newlines. When ``STRIP`` is specified, leading and trailing
   whitespaces will be removed. If ``REMOVE_EMPTY`` is specified, empty strings
   will be removed from the ``OUTPUT`` list.
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

   :OUTPUT[out]: List of generator expressions for C, CXX, and ASM compile
                 options

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

    set(compile_options)
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
   remove_duplicated_flags(<flags> <uniq_flags>)

   :flags[in]: Input string with compilation flags.
   :uniq_flags[out]: Output string with unified compilation flags.

   Remove duplicate entries from a string of compilation flags.
#]]
function(remove_duplicated_flags FLAGS UNIQFLAGS)
    set(FLAGS_LIST "${FLAGS}")
    # Convert the given flags, as a string, into a CMake list type
    separate_arguments(FLAGS_LIST)
    # Remove all the duplicated flags
    list(REMOVE_DUPLICATES FLAGS_LIST)
    # Convert the list back to a string
    string(REPLACE ";" " " FLAGS_LIST "${FLAGS_LIST}")
    # Return that string to the caller
    set(${UNIQFLAGS} "${FLAGS_LIST}" PARENT_SCOPE)
endfunction()

#[[
   spaces2list(<variable>)

   :variable[in,out]: String with space-delimited values.

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

   :variable[out]: Output variable with list of prefixed items.
   :prefix[in]: Prefix string.
   :item[in]: Items to be prefixed.

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

   :target[in]: Fail target name.
   :line0[in]: First line of the failed message.
   :line[in,opt]: Optional additional message lines.

   Create a phony target which fails when invoked. This is used when the
   necessary conditions for a target are not met, such as configuration. Rather
   than omitting the target altogether, we fail execution with a helpful
   message.
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

   :output[in]: Output file name.
   :INPUT[in]: Input file name.
   :CONTENT[in]: Input string.

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

   :old_target[in]: Old target name.
   :new_target[in]: New target name.

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

   :target[in]: Attach the source file with embedded data to the specified
                target.
   :embed_file[in]: File containing the embedded data.
   :embed_type[in]: BINARY or TEXT
   :RENAME_TO[in,opt]: Use the given symbol name for the embedded data. If no
                       symbol name is provided, the embed_file file name will
                       be used instead.
   :DEPENDS[in,opt]: List of additional dependencies for the generated file
                     containing embedded data.

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
