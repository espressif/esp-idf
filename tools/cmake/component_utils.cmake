function(debug message)
    if(DEBUG)
        message(STATUS "${message}")
    endif()
endfunction()

# Given a component name (find_name) and a list of component paths (component_paths),
# return the path to the component in 'variable'
#
# Fatal error is printed if the component is not found.
function(find_component_path find_name components component_paths variable)
    list(FIND components ${find_name} idx)
    if(NOT idx EQUAL -1)
        list(GET component_paths ${idx} path)
        set("${variable}" "${path}" PARENT_SCOPE)
        return()
    else()
    endif()
    # TODO: find a way to print the dependency chain that lead to this not-found component
    message(WARNING "Required component ${find_name} is not found in any of the provided COMPONENT_DIRS")
endfunction()

# components_find_all: Search 'component_dirs' for components and return them
# as a list of names in 'component_names' and a list of full paths in
# 'component_paths'
#
# component_paths contains only unique component names. Directories
# earlier in the component_dirs list take precedence.
function(components_find_all component_dirs component_paths component_names test_component_names)
    # component_dirs entries can be files or lists of files
    set(paths "")
    set(names "")
    set(test_names "")

    # start by expanding the component_dirs list with all subdirectories
    foreach(dir ${component_dirs})
        # Iterate any subdirectories for values
        file(GLOB subdirs LIST_DIRECTORIES true "${dir}/*")
        foreach(subdir ${subdirs})
            set(component_dirs "${component_dirs};${subdir}")
        endforeach()
    endforeach()

    # Look for a component in each component_dirs entry
    foreach(dir ${component_dirs})
        debug("Looking for CMakeLists.txt in ${dir}")
        file(GLOB component "${dir}/CMakeLists.txt")
        if(component)
            debug("CMakeLists.txt file ${component}")
            get_filename_component(component "${component}" DIRECTORY)
            get_filename_component(name "${component}" NAME)
            if(NOT name IN_LIST names)
                list(APPEND names "${name}")
                list(APPEND paths "${component}")

                # Look for test component directory
                file(GLOB test "${component}/test/CMakeLists.txt")
                if(test)
                    list(APPEND test_names "${name}")
                endif()
            endif()
        else()  # no CMakeLists.txt file
            # test for legacy component.mk and warn
            file(GLOB legacy_component "${dir}/component.mk")
            if(legacy_component)
                get_filename_component(legacy_component "${legacy_component}" DIRECTORY)
                message(WARNING "Component ${legacy_component} contains old-style component.mk but no CMakeLists.txt. "
                    "Component will be skipped.")
            endif()
        endif()
    endforeach()

    set(${component_paths} ${paths} PARENT_SCOPE)
    set(${component_names} ${names} PARENT_SCOPE)
    set(${test_component_names} ${test_names} PARENT_SCOPE)
endfunction()
