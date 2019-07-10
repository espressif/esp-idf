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
    message(FATAL_ERROR "Required component ${find_name} is not found in any of the provided COMPONENT_DIRS")
endfunction()

# components_find_all: Search 'component_dirs' for components and return them
# as a list of names in 'component_names' and a list of full paths in
# 'component_paths'
#
# component_paths contains only unique component names. Directories
# earlier in the component_dirs list take precedence.
function(components_find_all component_dirs component_paths component_names test_component_names)
    # component_dirs entries can be files or lists of files
    set(_paths "")
    set(_names "")
    set(_test_names "")

    # start by expanding the component_dirs list with all subdirectories
    foreach(dir ${component_dirs})
        if(EXISTS ${dir}/CMakeLists.txt)
            get_filename_component(_path "${dir}" ABSOLUTE)
            get_filename_component(_name "${_path}" NAME)
            if(NOT name IN_LIST names)
                list(APPEND _names "${_name}")
                list(APPEND _paths "${_path}")
            endif()

            if(EXISTS "${_path}/test/CMakeLists.txt")
                list(APPEND _test_names "${_name}")
            endif()
        else()
            if(EXISTS ${dir}/component.mk)
                get_filename_component(legacy_component "${legacy_component}" DIRECTORY)
                message(WARNING "Component ${legacy_component} contains old-style component.mk but no CMakeLists.txt. "
                    "Component will be skipped.")
            else()
                if(NOT __recursing) # recurse only once
                    file(GLOB subdirs LIST_DIRECTORIES true "${dir}/*")

                    set(__recursing 1)
                    components_find_all("${subdirs}" __paths __names __test_names)
                    set(__recursing 0)

                    if(__paths)
                        list(APPEND _paths "${__paths}")
                        list(APPEND _names "${__names}")
                        list(APPEND _test_names "${__test_names}")
                    endif()
                endif()
            endif()
        endif()
    endforeach()

    set(${test_component_names} "${_test_names}" PARENT_SCOPE)
    set(${component_paths} "${_paths}" PARENT_SCOPE)
    set(${component_names} "${_names}" PARENT_SCOPE)
endfunction()
