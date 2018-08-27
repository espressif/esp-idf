# expand_requires.cmake is a utility cmake script to expand component requirements early in the build,
# before the components are ready to be included.
#
# Parameters:
# - COMPONENTS = Space-separated list of initial components to include in the build.
#   Can be empty, in which case all components are in the build.
# - COMPONENT_REQUIRES_COMMON = Components to always include in the build, and treated as dependencies
#   of all other components.
# - DEPENDENCIES_FILE = Path of generated cmake file which will contain the expanded dependencies for these
#   components.
# - COMPONENT_DIRS = List of paths to search for all components.
# - DEBUG = Set -DDEBUG=1 to debug component lists in the build.
#
# If successful, DEPENDENCIES_FILE can be expanded to set BUILD_COMPONENTS & BUILD_COMPONENT_PATHS with all
# components required for the build, and the get_component_requirements() function to return each component's
# recursively expanded requirements.
#
# BUILD_COMPONENTS & BUILD_COMPONENT_PATHS will be ordered in a best-effort way so that dependencies are listed first.
# (Note that IDF supports cyclic dependencies, and dependencies in a cycle have ordering guarantees.)
#
# Determinism:
#
# Given the the same list of names in COMPONENTS (regardless of order), and an identical value of
# COMPONENT_REQUIRES_COMMON, and all the same COMPONENT_REQUIRES & COMPONENT_PRIV_REQUIRES values in
# each component, then the output of BUILD_COMPONENTS should always be in the same
# order.
#
# BUILD_COMPONENT_PATHS will be in the same component order as BUILD_COMPONENTS, even if the
# actual component paths are different due to different paths.
#
# TODO: Error out if a component requirement is missing
cmake_minimum_required(VERSION 3.5)
include("${IDF_PATH}/tools/cmake/utilities.cmake")

if(NOT DEPENDENCIES_FILE)
    message(FATAL_ERROR "DEPENDENCIES_FILE must be set.")
endif()

if(NOT COMPONENT_DIRS)
    message(FATAL_ERROR "COMPONENT_DIRS variable must be set")
endif()
spaces2list(COMPONENT_DIRS)

spaces2list(COMPONENT_REQUIRES_COMMON)

function(debug message)
    if(DEBUG)
        message(STATUS "${message}")
    endif()
endfunction()

# Dummy register_component used to save requirements variables as global properties, for later expansion
#
# (expand_component_requirements() includes the component CMakeLists.txt, which then sets its component variables,
# calls this dummy macro, and immediately exits again.)
macro(register_component)
    if(COMPONENT STREQUAL main AND NOT COMPONENT_REQUIRES)
        set(main_component_requires ${COMPONENTS})
        list(REMOVE_ITEM main_component_requires "main")

        set_property(GLOBAL PROPERTY "${COMPONENT}_REQUIRES" "${main_component_requires}")
    else()
        spaces2list(COMPONENT_REQUIRES)
        set_property(GLOBAL PROPERTY "${COMPONENT}_REQUIRES" "${COMPONENT_REQUIRES}")
    endif()

    spaces2list(COMPONENT_PRIV_REQUIRES)
    set_property(GLOBAL PROPERTY "${COMPONENT}_PRIV_REQUIRES" "${COMPONENT_PRIV_REQUIRES}")

    # This is tricky: we override register_component() so it returns out of the component CMakeLists.txt
    # (as we're declaring it as a macro not a function, so it doesn't have its own scope.)
    #
    # This means no targets are defined, and the component expansion ends early.
    return()
endmacro()

macro(register_config_only_component)
    register_component()
endmacro()

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


# expand_component_requirements: Recursively expand a component's requirements,
# setting global properties BUILD_COMPONENTS & BUILD_COMPONENT_PATHS and
# also invoking the components to call register_component() above,
# which will add per-component global properties with dependencies, etc.
function(expand_component_requirements component)
    get_property(seen_components GLOBAL PROPERTY SEEN_COMPONENTS)
    if(component IN_LIST seen_components)
        return()  # already added, or in process of adding, this component
    endif()
    set_property(GLOBAL APPEND PROPERTY SEEN_COMPONENTS ${component})

    find_component_path("${component}" "${ALL_COMPONENTS}" "${ALL_COMPONENT_PATHS}" component_path)
    debug("Expanding dependencies of ${component} @ ${component_path}")
    if(NOT component_path)
        set_property(GLOBAL APPEND PROPERTY COMPONENTS_NOT_FOUND ${component})
        return()
    endif()

    # include the component CMakeLists.txt to expand its properties
    # into the global cache (via register_component(), above)
    unset(COMPONENT_REQUIRES)
    unset(COMPONENT_PRIV_REQUIRES)
    set(COMPONENT ${component})
    include(${component_path}/CMakeLists.txt)

    get_property(requires GLOBAL PROPERTY "${component}_REQUIRES")
    get_property(requires_priv GLOBAL PROPERTY "${component}_PRIV_REQUIRES")

    # Recurse dependencies first, so that they appear first in the list (when possible)
    foreach(req ${COMPONENT_REQUIRES_COMMON} ${requires} ${requires_priv})
        expand_component_requirements(${req})
    endforeach()

    list(FIND TEST_COMPONENTS ${component} idx)

    if(NOT idx EQUAL -1)
        list(GET TEST_COMPONENTS ${idx} test_component)
        list(GET TEST_COMPONENT_PATHS ${idx} test_component_path)
        set_property(GLOBAL APPEND PROPERTY BUILD_TEST_COMPONENTS ${test_component})
        set_property(GLOBAL APPEND PROPERTY BUILD_TEST_COMPONENT_PATHS ${test_component_path})
    endif()

    # Now append this component to the full list (after its dependencies)
    set_property(GLOBAL APPEND PROPERTY BUILD_COMPONENT_PATHS ${component_path})
    set_property(GLOBAL APPEND PROPERTY BUILD_COMPONENTS ${component})
endfunction()

# filter_components_list: Filter the components included in the build
# as specified by the user. Or, in the case of unit testing, filter out
# the test components to be built.
macro(filter_components_list)
    spaces2list(COMPONENTS)
    spaces2list(EXCLUDE_COMPONENTS)
    spaces2list(TEST_COMPONENTS)
    spaces2list(TEST_EXCLUDE_COMPONENTS)

    list(LENGTH ALL_COMPONENTS all_components_length)
    math(EXPR all_components_length "${all_components_length} - 1")

    foreach(component_idx RANGE 0 ${all_components_length})
        list(GET ALL_COMPONENTS ${component_idx} component)
        list(GET ALL_COMPONENT_PATHS ${component_idx} component_path)

        if(COMPONENTS)
            if(${component} IN_LIST COMPONENTS)
                set(add_component 1)
            else()
                set(add_component 0)
            endif()
        else()
            set(add_component 1)

        endif()

        if(NOT ${component} IN_LIST EXCLUDE_COMPONENTS AND add_component EQUAL 1)
            list(APPEND components ${component})
            list(APPEND component_paths ${component_path})

            if(TESTS_ALL EQUAL 1 OR TEST_COMPONENTS)
                if(NOT TESTS_ALL EQUAL 1 AND TEST_COMPONENTS)
                    if(${component} IN_LIST TEST_COMPONENTS)
                        set(add_test_component 1)
                    else()
                        set(add_test_component 0)
                    endif()
                else()
                    set(add_test_component 1)
                endif()

                if(${component} IN_LIST ALL_TEST_COMPONENTS)
                    if(NOT ${component} IN_LIST TEST_EXCLUDE_COMPONENTS AND add_test_component EQUAL 1)
                        list(APPEND test_components ${component}_test)
                        list(APPEND test_component_paths ${component_path}/test)

                        list(APPEND components ${component}_test)
                        list(APPEND component_paths ${component_path}/test)
                    endif()
                endif()
            endif()
        endif()
    endforeach()

    set(COMPONENTS ${components})

    set(TEST_COMPONENTS ${test_components})
    set(TEST_COMPONENT_PATHS ${test_component_paths})

    list(APPEND ALL_COMPONENTS "${TEST_COMPONENTS}")
    list(APPEND ALL_COMPONENT_PATHS "${TEST_COMPONENT_PATHS}")
endmacro()

# Main functionality goes here
# Find every available component in COMPONENT_DIRS, save as ALL_COMPONENT_PATHS and ALL_COMPONENTS
components_find_all("${COMPONENT_DIRS}" ALL_COMPONENT_PATHS ALL_COMPONENTS ALL_TEST_COMPONENTS)

filter_components_list()

debug("ALL_COMPONENT_PATHS ${ALL_COMPONENT_PATHS}")
debug("ALL_COMPONENTS ${ALL_COMPONENTS}")
debug("ALL_TEST_COMPONENTS ${ALL_TEST_COMPONENTS}")

set_property(GLOBAL PROPERTY SEEN_COMPONENTS "")  # anti-infinite-recursion
set_property(GLOBAL PROPERTY BUILD_COMPONENTS "")
set_property(GLOBAL PROPERTY BUILD_COMPONENT_PATHS "")
set_property(GLOBAL PROPERTY BUILD_TEST_COMPONENTS "")
set_property(GLOBAL PROPERTY BUILD_TEST_COMPONENT_PATHS "")
set_property(GLOBAL PROPERTY COMPONENTS_NOT_FOUND "")

# Indicate that the component CMakeLists.txt is being included in the early expansion phase of the build,
# and might not want to execute particular operations.
set(CMAKE_BUILD_EARLY_EXPANSION 1)
foreach(component ${COMPONENTS})
    debug("Expanding initial component ${component}")
    expand_component_requirements(${component})
endforeach()
unset(CMAKE_BUILD_EARLY_EXPANSION)

get_property(build_components GLOBAL PROPERTY BUILD_COMPONENTS)
get_property(build_component_paths GLOBAL PROPERTY BUILD_COMPONENT_PATHS)
get_property(build_test_components GLOBAL PROPERTY BUILD_TEST_COMPONENTS)
get_property(build_test_component_paths GLOBAL PROPERTY BUILD_TEST_COMPONENT_PATHS)
get_property(not_found GLOBAL PROPERTY COMPONENTS_NOT_FOUND)

debug("components in build: ${build_components}")
debug("components in build: ${build_component_paths}")
debug("components not found: ${not_found}")

function(line contents)
    file(APPEND "${DEPENDENCIES_FILE}.tmp" "${contents}\n")
endfunction()

file(WRITE "${DEPENDENCIES_FILE}.tmp" "# Component requirements generated by expand_requirements.cmake\n\n")
line("set(BUILD_COMPONENTS ${build_components})")
line("set(BUILD_COMPONENT_PATHS ${build_component_paths})")
line("set(BUILD_TEST_COMPONENTS ${build_test_components})")
line("set(BUILD_TEST_COMPONENT_PATHS ${build_test_component_paths})")
line("")

line("# get_component_requirements: Generated function to read the dependencies of a given component.")
line("#")
line("# Parameters:")
line("# - component: Name of component")
line("# - var_requires: output variable name. Set to recursively expanded COMPONENT_REQUIRES ")
line("#   for this component.")
line("# - var_private_requires: output variable name. Set to recursively expanded COMPONENT_PRIV_REQUIRES ")
line("#   for this component.")
line("#")
line("# Throws a fatal error if 'componeont' is not found (indicates a build system problem).")
line("#")
line("function(get_component_requirements component var_requires var_private_requires)")
foreach(build_component ${build_components})
    get_property(reqs GLOBAL PROPERTY "${build_component}_REQUIRES")
    get_property(private_reqs GLOBAL PROPERTY "${build_component}_PRIV_REQUIRES")
    line("  if(\"\$\{component}\" STREQUAL \"${build_component}\")")
    line("    set(\${var_requires} \"${reqs}\" PARENT_SCOPE)")
    line("    set(\${var_private_requires} \"${private_reqs}\" PARENT_SCOPE)")
    line("    return()")
    line("  endif()")
endforeach()

line("  message(FATAL_ERROR \"Component not found: \${component}\")")
line("endfunction()")

# only replace DEPENDENCIES_FILE if it has changed (prevents ninja/make build loops.)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEPENDENCIES_FILE}.tmp" "${DEPENDENCIES_FILE}")
execute_process(COMMAND ${CMAKE_COMMAND} -E remove "${DEPENDENCIES_FILE}.tmp")
