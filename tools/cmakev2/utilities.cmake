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
   2. CMake cache variable
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
    elseif(DEFINED CACHE{${ARG_VARIABLE}})
        set(value "$CACHE{${ARG_VARIABLE}}")
    else()
        set(value "${ARG_DEFAULT}")
    endif()

    set(${ARG_OUTPUT} "${value}" PARENT_SCOPE)
endfunction()
