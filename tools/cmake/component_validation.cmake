#
# Component validation checks
#
# This module contains checks that validate component source files and include directories
# to ensure they belong to the correct component. These checks run after all components
# have been discovered and processed.
#

#
# Check if a path belongs to a specific component
#
function(__component_validation_get_component_for_path var path)
    # Determine the starting directory to check: use the path itself if it's a directory,
    # otherwise use its containing directory
    set(current_dir "${path}")
    if(NOT IS_DIRECTORY "${current_dir}")
        get_filename_component(current_dir "${path}" DIRECTORY)
    endif()

    # Get all component targets
    idf_build_get_property(component_targets __COMPONENT_TARGETS)

    # Walk up the directory tree from the deepest path towards root and return
    # the first component whose COMPONENT_DIR matches exactly. This guarantees
    # selecting the deepest matching component without extra heuristics.
    while(NOT "${current_dir}" STREQUAL "" AND
          NOT "${current_dir}" STREQUAL "/" AND
          NOT "${current_dir}" MATCHES "^[A-Za-z]:/$")
        foreach(component_target ${component_targets})
            __component_get_property(component_dir ${component_target} COMPONENT_DIR)
            if(current_dir STREQUAL component_dir)
                set(${var} ${component_target} PARENT_SCOPE)
                return()
            endif()
        endforeach()
        get_filename_component(current_dir "${current_dir}" DIRECTORY)
    endwhile()

    # If no component found, return empty
    set(${var} "" PARENT_SCOPE)
endfunction()

#
# Validate that source files belong to the correct component
#
function(__component_validation_check_sources component_target)
    __component_get_property(sources ${component_target} SRCS)
    __component_get_property(component_name ${component_target} COMPONENT_NAME)
    __component_get_property(component_dir ${component_target} COMPONENT_DIR)

    foreach(src ${sources})
        # Check if this source file belongs to another component
        __component_validation_get_component_for_path(owner_component ${src})

        if(owner_component AND NOT owner_component STREQUAL component_target)
            __component_get_property(owner_name ${owner_component} COMPONENT_NAME)
            message(WARNING
                "Source file '${src}' belongs to component ${owner_name} but is being built by "
                "component ${component_name}. It is recommended to build source files by "
                "defining component dependencies for ${component_name} "
                "via using idf_component_register(REQUIRES ${owner_name}) "
                "or idf_component_register(PRIV_REQUIRES ${owner_name}) in the CMakeLists.txt of "
                "${component_name}.")
        endif()
    endforeach()
endfunction()

#
# Validate that include directories belong to the correct component
#
function(__component_validation_check_include_dirs component_target)
    __component_get_property(include_dirs ${component_target} INCLUDE_DIRS)
    __component_get_property(priv_include_dirs ${component_target} PRIV_INCLUDE_DIRS)
    __component_get_property(component_name ${component_target} COMPONENT_NAME)
    __component_get_property(component_dir ${component_target} COMPONENT_DIR)

    # Check public include directories
    foreach(dir ${include_dirs})
        # Check if this include directory belongs to another component
        # Normalize to absolute path relative to this component directory
        get_filename_component(abs_dir ${dir} ABSOLUTE BASE_DIR ${component_dir})
        __component_validation_get_component_for_path(owner_component ${abs_dir})

        if(owner_component AND NOT owner_component STREQUAL component_target)
            __component_get_property(owner_name ${owner_component} COMPONENT_NAME)
            message(WARNING
                "Include directory '${abs_dir}' belongs to component ${owner_name} but is being "
                "used by component ${component_name}. It is recommended to define the "
                "component dependency for '${component_name}' on the component ${owner_name}, "
                "i.e. 'idf_component_register(... REQUIRES ${owner_name})' in the "
                "CMakeLists.txt of ${component_name}, and specify the included directory "
                "as idf_component_register(... INCLUDE_DIRS <dir relative to component>) "
                "in the CMakeLists.txt of component ${owner_name}.")
        endif()
    endforeach()

    # Check private include directories
    foreach(dir ${priv_include_dirs})
        # Check if this include directory belongs to another component
        # Normalize to absolute path relative to this component directory
        get_filename_component(abs_dir ${dir} ABSOLUTE BASE_DIR ${component_dir})
        __component_validation_get_component_for_path(owner_component ${abs_dir})

        if(owner_component AND NOT owner_component STREQUAL component_target)
            __component_get_property(owner_name ${owner_component} COMPONENT_NAME)
            message(WARNING
                "Private include directory '${abs_dir}' belongs to component ${owner_name} but "
                "is being used by component ${component_name}. "
                "It is recommended to define the component dependency for ${component_name} "
                "on the component ${owner_name}, "
                "i.e. 'idf_component_register(... PRIV_REQUIRES ${owner_name})' in the "
                "CMakeLists.txt of ${component_name}, "
                "and specify the included directory as "
                "idf_component_register(... PRIV_INCLUDE_DIRS <dir relative to component>) "
                "in the CMakeLists.txt of component ${owner_name}.")
        endif()
    endforeach()
endfunction()

#
# Run validation checks for all components
#
function(__component_validation_run_checks)
    # Get all component targets
    idf_build_get_property(component_targets __BUILD_COMPONENT_TARGETS)

    # Run validation checks for each component
    foreach(component_target ${component_targets})
        __component_validation_check_sources(${component_target})
        __component_validation_check_include_dirs(${component_target})
    endforeach()
endfunction()
