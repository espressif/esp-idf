# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Component validation checks
#
# This module contains checks that validate component source files and include directories
# to ensure they belong to the correct component. These checks run after all components
# have been discovered and processed.

#[[
    __component_validation_get_component_for_path(<var> <path>)

    *var[out]*

        Output variable to store the component name.

    *path[in]*

        File or directory path to check.

    Determine which component a given path belongs to. Returns the component name in ``var``.
#]]
function(__component_validation_get_component_for_path var path)
    # Determine the starting directory to check: use the path itself if it's a directory,
    # otherwise use its containing directory
    set(current_dir "${path}")
    if(NOT IS_DIRECTORY "${current_dir}")
        get_filename_component(current_dir "${path}" DIRECTORY)
    endif()

    # Get all component names
    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)

    # Walk up the directory tree from the deepest path towards root and return
    # the first component whose COMPONENT_DIR matches exactly. This guarantees
    # selecting the deepest matching component without extra heuristics.
    while(NOT "${current_dir}" STREQUAL "" AND
          NOT "${current_dir}" STREQUAL "/" AND
          NOT "${current_dir}" MATCHES "^[A-Za-z]:/$")
        foreach(component_interface ${component_interfaces})
            __idf_component_get_property_unchecked(component_dir ${component_interface} COMPONENT_DIR)
            if(current_dir STREQUAL component_dir)
                __idf_component_get_property_unchecked(component_name ${component_interface} COMPONENT_NAME)
                set(${var} ${component_name} PARENT_SCOPE)
                return()
            endif()
        endforeach()
        get_filename_component(current_dir "${current_dir}" DIRECTORY)
    endwhile()

    # If no component found, return empty
    set(${var} "" PARENT_SCOPE)
endfunction()

#[[
    __component_validation_check_sources(<component_interface>)

    *component_interface[in]*

        Component interface to validate.

    Validate that all source files of the specified component belong to that
    component's directory tree. Issues warnings if source files belong to other
    components.
#]]
function(__component_validation_check_sources component_interface)
    __idf_component_get_property_unchecked(component_name ${component_interface} COMPONENT_NAME)
    __idf_component_get_property_unchecked(sources ${component_interface} SRCS)

    # Skip validation if component does not have any sources
    if(NOT sources)
        return()
    endif()

    foreach(src ${sources})
        # Check if this source file belongs to another component
        __component_validation_get_component_for_path(owner_component ${src})

        if(owner_component AND NOT owner_component STREQUAL component_name)
            idf_warn(
                "Source file '${src}' belongs to component ${owner_component} but is being built by "
                "component ${component_name}. It is recommended to build source files by "
                "defining component dependencies for ${component_name} "
                "via using idf_component_register(REQUIRES ${owner_component}) "
                "or idf_component_register(PRIV_REQUIRES ${owner_component}) in the CMakeLists.txt of "
                "${component_name}.")
        endif()
    endforeach()
endfunction()

#[[
    __component_validation_check_include_dirs(<component_name>)

    *component_name[in]*

        Component name to validate.

    Validate that all include directories (INCLUDE_DIRS and PRIV_INCLUDE_DIRS
    properties) of the specified component belong to that component's directory
    tree. Issues warnings if include directories belong to other components.
#]]
function(__component_validation_check_include_dirs component_interface)
    __idf_component_get_property_unchecked(component_name ${component_interface} COMPONENT_NAME)
    __idf_component_get_property_unchecked(include_dirs ${component_interface} INCLUDE_DIRS)
    __idf_component_get_property_unchecked(priv_include_dirs ${component_interface} PRIV_INCLUDE_DIRS)
    __idf_component_get_property_unchecked(component_dir ${component_interface} COMPONENT_DIR)

    # Check public include directories
    foreach(dir ${include_dirs})
        # Check if this include directory belongs to another component
        # Normalize to absolute path relative to this component directory
        get_filename_component(abs_dir ${dir} ABSOLUTE BASE_DIR ${component_dir})
        __component_validation_get_component_for_path(owner_component ${abs_dir})

        if(owner_component AND NOT owner_component STREQUAL component_name)
            idf_warn(
                "Include directory '${abs_dir}' belongs to component ${owner_component} but is being "
                "used by component ${component_name}. It is recommended to define the "
                "component dependency for '${component_name}' on the component ${owner_component}, "
                "i.e. 'idf_component_register(... REQUIRES ${owner_component})' in the "
                "CMakeLists.txt of ${component_name}, and specify the included directory "
                "as idf_component_register(... INCLUDE_DIRS <dir relative to component>) "
                "in the CMakeLists.txt of component ${owner_component}.")
        endif()
    endforeach()

    # Check private include directories
    foreach(dir ${priv_include_dirs})
        # Check if this include directory belongs to another component
        # Normalize to absolute path relative to this component directory
        get_filename_component(abs_dir ${dir} ABSOLUTE BASE_DIR ${component_dir})
        __component_validation_get_component_for_path(owner_component ${abs_dir})

        if(owner_component AND NOT owner_component STREQUAL component_name)
            idf_warn(
                "Private include directory '${abs_dir}' belongs to component ${owner_component} but "
                "is being used by component ${component_name}. "
                "It is recommended to define the component dependency for ${component_name} "
                "on the component ${owner_component}, "
                "i.e. 'idf_component_register(... PRIV_REQUIRES ${owner_component})' in the "
                "CMakeLists.txt of ${component_name}, "
                "and specify the included directory as "
                "idf_component_register(... PRIV_INCLUDE_DIRS <dir relative to component>) "
                "in the CMakeLists.txt of component ${owner_component}.")
        endif()
    endforeach()
endfunction()

#[[
    __component_validation_run_checks(LIBRARY <library>)

    *library[in]*

        Library name for which to run validation checks.

    Run validation checks for all components linked to the specified library.
#]]
function(__component_validation_run_checks)
    set(options)
    set(one_value LIBRARY)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_LIBRARY)
        idf_die("LIBRARY option is required for __component_validation_run_checks")
    endif()

    # Validate only components linked to the specified library
    idf_library_get_property(component_interfaces "${ARG_LIBRARY}" LIBRARY_COMPONENT_INTERFACES_LINKED)

    # Run validation checks for each component
    foreach(component_interface ${component_interfaces})
        __component_validation_check_sources(${component_interface})
        __component_validation_check_include_dirs(${component_interface})
    endforeach()
endfunction()
