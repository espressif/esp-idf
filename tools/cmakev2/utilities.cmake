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
                        OUTPUT <variable>)

   :PATHS[in]: List of paths to convert to absolute paths.
   :OUTPUT[out]: Output variable to store absolute paths.

   For a given ``PATHS``, return the absolute paths in ``OUTPUT``.
#]]
function(__get_absolute_paths)
    set(options)
    set(one_value OUTPUT)
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
        get_filename_component(path_abs ${path} ABSOLUTE)
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
