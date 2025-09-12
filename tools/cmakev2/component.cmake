# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)
include(build)
include(kconfig)

#[[api
.. cmakev2:function:: idf_component_set_property

    .. code-block:: cmake

        idf_component_set_property(<component> <property> <value> [APPEND])

    *component[in]*

        Component name, target, target alias or interface.

    *property[in]*

        Property name.

    *value[in]*

        Property value.

    *APPEND*
        Append the value to the property's current value instead of replacing
        it.

    Set the value of the specified component property. The property is also
    added to the internal list of component properties if it isn't already
    there.
#]]
function(idf_component_set_property component property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(append)
    if(ARG_APPEND)
        set(append APPEND)
    endif()

    __get_component_interface_or_die(COMPONENT "${component}" OUTPUT component_interface)
    __set_property(TARGET "${component_interface}"
                   PROPERTY "${property}"
                   PROPERTIES COMPONENT_PROPERTIES
                   VALUE "${value}"
                   ${append})
endfunction()

#[[api
.. cmakev2:function:: idf_component_get_property

    .. code-block:: cmake

        idf_component_get_property(<variable> <component> <property> [GENERATOR_EXPRESSION])

    *variable[out]*

        Variable to store the value in.

    *component[in]*

        Component name, target, target alias or interface.

    *property[in]*

        Property name to get the value of.

    *GENERATOR_EXPRESSION*

        Obtain the generator expression for the property rather than the actual
        value.

    Retrieve the value of the specified component property.
#]]
function(idf_component_get_property variable component property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(genexpr)
    if(ARG_GENERATOR_EXPRESSION)
        set(genexpr GENERATOR_EXPRESSION)
    endif()

    __get_component_interface_or_die(COMPONENT "${component}" OUTPUT component_interface)
    __get_property(TARGET "${component_interface}"
                   PROPERTY "${property}"
                   OUTPUT value
                   ${genexpr})
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
    __get_component_paths(PATHS <path>...
                          [EXCLUDE_PATHS <path>...]
                          [SOURCE <source>]
                          [CHECK]
                          OUTPUT <var>)

    *PATHS[in]*

        List of paths to search for component directories.

    *EXCLUDE_PATHS[in,opt]*

        Optional list of paths to exclude from the search of component
        directories.

    *SOURCE[in,opt]*

        Source of the ``PATHS``. If provided, it will be included in the error
        message when ``CHECK`` is specified.

    *CHECK[in,opt]*

        Verify whether the paths listed in "PATHS" exist. If any path is
        missing, abort the build process.

    *OUTPUT[out]*

        Output variable to store the list of found component directories.

    Search for component directories in the specified ``PATHS``, excluding
    those in ``EXCLUDE_PATHS``, and store the list of absolute component paths
    in the ``OUTPUT`` variable. If ``CHECK`` is specified, ensure that the
    paths listed in ``PATHS`` exist, and stop the build process if they do not.
#]]
function(__get_component_paths)
    set(options CHECK)
    set(one_value SOURCE OUTPUT)
    set(multi_value PATHS EXCLUDE_PATHS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_PATHS)
        idf_die("PATHS option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    __get_absolute_paths(PATHS "${ARG_PATHS}" OUTPUT include_paths)
    __get_absolute_paths(PATHS "${ARG_EXCLUDE_PATHS}" OUTPUT exclude_paths)

    if(ARG_CHECK)
        foreach(path IN LISTS include_paths)
            if(NOT IS_DIRECTORY "${path}")
                if(DEFINED ARG_SOURCE)
                    idf_die("Directory specified in '${ARG_SOURCE}' doesn't exist: '${path}'")
                else()
                    idf_die("Directory doesn't exist: '${path}'")
                endif()
            endif()
        endforeach()
    endif()

    set(paths "${include_paths}")
    set(component_paths "")

    while(paths)
        list(POP_FRONT paths path)

        if(NOT IS_DIRECTORY "${path}" OR "${path}" IN_LIST exclude_paths)
            continue()
        endif()

        if(EXISTS "${path}/CMakeLists.txt")
            list(APPEND component_paths "${path}")
        elseif("${path}" IN_LIST include_paths)
            file(GLOB dirs "${path}/*")
            __get_absolute_paths(PATHS "${dirs}" OUTPUT dirs_abs)
            list(APPEND paths "${dirs_abs}")
        endif()

    endwhile()

    set(${ARG_OUTPUT} "${component_paths}" PARENT_SCOPE)
endfunction()

#[[
    __get_component_interface(COMPONENT <component>
                              OUTPUT <variable>)

    *COMPONENT[int]*

        Component name, target, target alias or interface.

    *OUTPUT[out]*

        Output variable to store the component interface.

    Identify the component interface target using the ``<component>`` value,
    which could be a component name, target, target alias, or interface. Return
    the component interface target, or NOTFOUND if the interface cannot be
    located.

    component interface: <component_prefix>_<component name>
    component target: _<component_prefix>_<component name>
    component alias: <component_prefix>::<component name>
#]]
function(__get_component_interface)
    set(options)
    set(one_value COMPONENT OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_COMPONENT)
        idf_die("COMPONENT option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    idf_build_get_property(component_names COMPONENTS_DISCOVERED)
    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
    idf_build_get_property(component_prefix PREFIX)

    set(component_interface NOTFOUND)

    # Try to resolve component name from short name to namespaced name
    __resolve_component_name(COMPONENT "${ARG_COMPONENT}"
                            KNOWN_COMPONENTS "${component_names}"
                            OUTPUT resolved_component)

    if("${resolved_component}" IN_LIST component_names)
        # Found a resolved component name, use it
        set(component_interface "${component_prefix}_${resolved_component}")
    else()
        # The component name might be an alias, so retrieve the actual target
        # name.
        __get_real_target(TARGET ${ARG_COMPONENT} OUTPUT real_target)
        if("${real_target}" IN_LIST component_interfaces)
            # The component name is already a component interface or its alias.
            set(component_interface "${real_target}")
        else()
            string(SUBSTRING "${ARG_COMPONENT}" 1 -1 interface)
            if("${interface}" IN_LIST component_interfaces)
                # The component name is the actual target of the component.
                set(component_interface "${interface}")
            endif()
        endif()
    endif()

    # Sanity check
    if(NOT "${component_interface}" STREQUAL "NOTFOUND"
       AND NOT "${component_interface}" IN_LIST component_interfaces)
        idf_warn("Interface target '${component_interface}' found for component "
                 "'${ARG_COMPONENT}', but it's not present in the component "
                 "interface list.")
        set(component_interface NOTFOUND)
    endif()

    set(${ARG_OUTPUT} ${component_interface} PARENT_SCOPE)
endfunction()

#[[
    __get_component_interface_or_die(COMPONENT <component>
                                     OUTPUT <variable>)

    *COMPONENT[in]*

        Component name, target, target alias or interface.

    *OUTPUT[out]*

        Output variable to store the component interface.

    A simple wrapper for ``__get_component_interface`` that aborts the build
    process if the component interface is not found.
#]]
function(__get_component_interface_or_die)
    set(options)
    set(one_value COMPONENT OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_COMPONENT)
        idf_die("COMPONENT option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    __get_component_interface(COMPONENT "${ARG_COMPONENT}" OUTPUT component_interface)
    if("${component_interface}" STREQUAL "NOTFOUND")
        idf_die("Component interface for component '${ARG_COMPONENT}' does not exist")
    endif()
    set(${ARG_OUTPUT} ${component_interface} PARENT_SCOPE)
endfunction()

#[[
    __get_component_priority(SOURCE <source>
                             OUTPUT <variable>)

    *SOURCE[in]*

        String identifying the component source.

    *OUTPUT[out]*

        Output variable to store the component priority.

    Return the priority number of a component, where a higher number indicates
    a higher priority, based on the given ``source`` string. If the ``source``
    is not valid, return ``NOTFOUND``.
#]]
function(__get_component_priority)
    set(options)
    set(one_value SOURCE OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_SOURCE)
        idf_die("SOURCE option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if("${ARG_SOURCE}" STREQUAL "project_components")
        set(priority 3)
    elseif("${ARG_SOURCE}" STREQUAL "project_extra_components")
        set(priority 2)
    elseif("${ARG_SOURCE}" STREQUAL "project_managed_components")
        set(priority 1)
    elseif("${ARG_SOURCE}" STREQUAL "idf_components")
        set(priority 0)
    else()
        set(priority NOTFOUND)
    endif()

    set(${ARG_OUTPUT} "${priority}" PARENT_SCOPE)
endfunction()

#[[
    __init_component(DIRECTORY <path>
                     PREFIX <prefix>
                     SOURCE <source>)

    *DIRECTORY[in]*

        ``<path>`` where the component is located.

    *PREFIX[in]*

        Prefix for component target names.

    *SOURCE[in]*

        String identifying the component source.

    Initialize the component by creating a component interface target, allowing
    properties to be attached to it and also add initial component properties.
    At this stage, the component is not included in the build. The actual
    component target, named as specified in the `COMPONENT_LIB` property, is
    created later when ``add_subdirectory`` is called for the component's
    directory based on the project or other component requirements.

    Components are identified by the directory name they reside in. This means
    that components with the same name might exist in different directory
    paths.  In such cases, the component with the higher priority is used.
    Priority is determined by the component's source, as defined in
    ``__get_component_priority``. If a component with a higher priority than an
    existing one is initialized, its name, targets, and other properties remain
    the same. Only the directory, priority, and source are updated in the
    already initialized component.

    The name of the initialized component is added to the
    ``COMPONENTS_DISCOVERED`` build property, and its interface target is added
    to the ``COMPONENT_INTERFACES`` build property. These two lists are used by
    ``__get_component_interface``, which searches for a component interface
    based on the component name, alias, or targets, enabling the setting and
    retrieval of component properties.
#]]
function(__init_component)
    set(options)
    set(one_value DIRECTORY PREFIX SOURCE)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_DIRECTORY)
        idf_die("DIRECTORY option is required")
    endif()

    if(NOT DEFINED ARG_SOURCE)
        idf_die("SOURCE option is required")
    endif()

    if(NOT DEFINED ARG_PREFIX)
        idf_die("PREFIX option is required")
    endif()

    set(component_source "${ARG_SOURCE}")
    set(component_prefix "${ARG_PREFIX}")
    get_filename_component(component_directory ${ARG_DIRECTORY} ABSOLUTE)
    get_filename_component(component_name ${component_directory} NAME)
    __get_component_priority(SOURCE "${component_source}" OUTPUT component_priority)

    if(NOT EXISTS "${component_directory}/CMakeLists.txt")
        idf_die("Directory '${component_directory}' does not contain a component")
    endif()

    if("${component_priority}" STREQUAL "NOTFOUND")
        idf_die("Unknown component source '${component_source}' "
                "for directory '${component_directory}'")
    endif()

    # Collect Kconfig files from this component directory.
    idf_build_get_property(target IDF_TARGET)
    __collect_kconfig_files_from_directory("${component_directory}" "${target}"
        component_kconfig component_projbuild component_rename)

    set(component_project_include "${component_directory}/project_include.cmake")
    if(NOT EXISTS "${component_project_include}")
        set(component_project_include "")
    endif()

    # Track manifest presence
    if(EXISTS "${component_directory}/idf_component.yml")
        idf_build_set_property(__COMPONENTS_WITH_MANIFESTS "${component_directory}" APPEND)
    endif()

    __get_component_interface(COMPONENT "${component_name}" OUTPUT existing_component_interface)
    if(NOT "${existing_component_interface}" STREQUAL "NOTFOUND")
        # A component with the same name is already initialized. Check if it
        # should be replaced with the component currently being initialized.
        idf_component_get_property(existing_component_priority
                                   "${existing_component_interface}"
                                   COMPONENT_PRIORITY)
        idf_component_get_property(existing_component_directory
                                   "${existing_component_interface}"
                                   COMPONENT_DIR)
        idf_component_get_property(existing_component_source
                                   "${existing_component_interface}"
                                   COMPONENT_SOURCE)

        if(${component_priority} EQUAL ${existing_component_priority})
            idf_die("Component directory '${component_directory}' has the same "
                    "priority '${component_source}' as component directory "
                    "'${existing_component_directory}'")

        elseif(${component_priority} LESS ${existing_component_priority})
            idf_warn("Component directory '${component_directory}' has lower "
                     "priority '${component_source}' than component directory "
                     "'${existing_component_directory}' with priority "
                     "'${existing_component_source}' and will be ignored")
        else()
            idf_warn("Component '${component_name}' directory '${component_directory}' "
                     "with higher priority '${component_source}' will be used instead of "
                     "component directory '${existing_component_directory}' "
                     "with lower priority '${existing_component_source}'")
            # The newly added component has a higher priority than the existing
            # one. Since the component name and targets are identical, update
            # the existing component with the new directory and priority.
            idf_component_set_property("${component_name}" COMPONENT_DIR "${component_directory}")
            idf_component_set_property("${component_name}" COMPONENT_SOURCE "${component_source}")
            idf_component_set_property("${component_name}" COMPONENT_PRIORITY ${component_priority})

            # Update component properties with new Kconfig files
            idf_component_set_property("${component_name}" __KCONFIG "${component_kconfig}")
            idf_component_set_property("${component_name}" __KCONFIG_PROJBUILD "${component_projbuild}")
            idf_component_set_property("${component_name}" __SDKCONFIG_RENAME "${component_rename}")

            # Update component project_include.cmake path.
            idf_component_set_property("${component_name}" __PROJECT_INCLUDE "${component_project_include}")
        endif()

        return()
    endif()

    set(component_interface "${component_prefix}_${component_name}")
    set(component_alias "${component_prefix}::${component_name}")
    # Real component library target that needs to be created by the component.
    set(component_target "_${component_interface}")

    # Interface target is used to attach all component properties and is also
    # used when the component is linked to other targets.
    add_library("${component_interface}" INTERFACE)

    idf_build_set_property(COMPONENTS_DISCOVERED "${component_name}" APPEND)
    idf_build_set_property(COMPONENT_INTERFACES "${component_interface}" APPEND)

    idf_component_set_property("${component_name}" COMPONENT_LIB "${component_target}")
    idf_component_set_property("${component_name}" COMPONENT_TARGET "${component_target}")
    idf_component_set_property("${component_name}" COMPONENT_NAME "${component_name}")
    idf_component_set_property("${component_name}" COMPONENT_DIR "${component_directory}")
    idf_component_set_property("${component_name}" COMPONENT_ALIAS "${component_alias}")
    idf_component_set_property("${component_name}" COMPONENT_SOURCE "${component_source}")
    idf_component_set_property("${component_name}" COMPONENT_INTERFACE "${component_interface}")
    idf_component_set_property("${component_name}" COMPONENT_PRIORITY ${component_priority})
    idf_component_set_property("${component_name}" COMPONENT_INCLUDED NO)
    idf_component_set_property("${component_name}" __PREFIX "${ARG_PREFIX}")

    # Set component properties for Kconfig files.
    idf_component_set_property("${component_name}" __KCONFIG "${component_kconfig}")
    idf_component_set_property("${component_name}" __KCONFIG_PROJBUILD "${component_projbuild}")
    idf_component_set_property("${component_name}" __SDKCONFIG_RENAME "${component_rename}")

    # Set component project_include.cmake.
    idf_component_set_property("${component_name}" __PROJECT_INCLUDE "${component_project_include}")
endfunction()

#[[
    __dump_component_properties(<components>)

    *components[in]*

        List of components whose properties should be displayed.

    Dump all properties for the components listed in ``components``.
#]]
function(__dump_component_properties components)
    foreach(component IN LISTS components)
        idf_component_get_property(properties "${component}" COMPONENT_PROPERTIES)
        idf_msg("component '${component}' properties: ${properties}")
        foreach(property IN LISTS properties)
            idf_component_get_property(value "${component}" "${property}")
            idf_msg("   ${property}: ${value}")
        endforeach()
    endforeach()
endfunction()

#[[
    __component_name_without_namespace(COMPONENT <component_name>
                                       OUTPUT <variable>)

    *COMPONENT[in]*

        Component name that may include namespace.

    *OUTPUT[out]*

        Output variable to store the component name without namespace.

    Extract the component name without namespace. For example:
    - "espressif__led_strip" -> "led_strip"
    - "led_strip" -> "led_strip" This follows the same logic as the component
    manager's name_without_namespace function.
#]]
function(__component_name_without_namespace)
    set(options)
    set(one_value COMPONENT OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_COMPONENT)
        idf_die("COMPONENT option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    # Split by "__" and take the last part (same logic as component manager)
    # Use rsplit with maxsplit=1 equivalent: split and take last part
    string(FIND "${ARG_COMPONENT}" "__" last_pos REVERSE)
    if(last_pos GREATER -1)
        math(EXPR start_pos "${last_pos} + 2")
        string(SUBSTRING "${ARG_COMPONENT}" ${start_pos} -1 name_without_ns)
    else()
        set(name_without_ns "${ARG_COMPONENT}")
    endif()

    set(${ARG_OUTPUT} "${name_without_ns}" PARENT_SCOPE)
endfunction()

#[[
    __resolve_component_name(COMPONENT <component_name>
                             KNOWN_COMPONENTS <known_components_list>
                             OUTPUT <variable>)

    *COMPONENT[in]*

        Component name to resolve (may be short name or canonical).

    *KNOWN_COMPONENTS[in]*

        List of known/discovered component names.

    *OUTPUT[out]*

        Output variable to store the resolved component name.

    Resolve a component name to its canonical form by checking against known components.
    This implements the same logic as the component manager's _choose_component function:
    1. If the exact name exists in known components, return it
    2. If a namespaced version exists (e.g., "espressif__led_strip" for "led_strip"), return that
    3. If a non-namespaced version exists (e.g., "led_strip" for "espressif__led_strip"), return that
    4. If any known component has the same name without namespace , return that
    5. Otherwise, return the original name (will likely cause CMake error)

    This function takes component priority into account by preferring
    components with higher priority when multiple matches exist. This behavior
    differs from the component manager's _choose_component function.
#]]
function(__resolve_component_name)
    set(options)
    set(one_value COMPONENT OUTPUT)
    set(multi_value KNOWN_COMPONENTS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_COMPONENT)
        idf_die("COMPONENT option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    set(component "${ARG_COMPONENT}")
    set(known_components "${ARG_KNOWN_COMPONENTS}")

    # 1. If exact name exists, return it (highest priority match)
    if("${component}" IN_LIST known_components)
        set(${ARG_OUTPUT} "${component}" PARENT_SCOPE)
        return()
    endif()

    # 2. Check for namespaced version (e.g., "led_strip" -> "espressif__led_strip")
    # Look for components ending with "__${component}"
    set(namespaced_suffix "__${component}")
    set(best_match "")
    set(best_priority -1)

    foreach(known_component IN LISTS known_components)
        string(LENGTH "${namespaced_suffix}" suffix_len)
        string(LENGTH "${known_component}" comp_len)
        if(comp_len GREATER_EQUAL suffix_len)
            math(EXPR start_pos "${comp_len} - ${suffix_len}")
            string(SUBSTRING "${known_component}" ${start_pos} -1 comp_suffix)
            if("${comp_suffix}" STREQUAL "${namespaced_suffix}")
                # Get component priority to choose the best match
                idf_component_get_property(comp_priority "${known_component}" COMPONENT_PRIORITY)
                if(comp_priority GREATER best_priority)
                    set(best_match "${known_component}")
                    set(best_priority ${comp_priority})
                endif()
            endif()
        endif()
    endforeach()

    if(NOT "${best_match}" STREQUAL "")
        idf_dbg("Component '${component}' resolved to namespaced version '${best_match}'")
        set(${ARG_OUTPUT} "${best_match}" PARENT_SCOPE)
        return()
    endif()

    # 3. Check for non-namespaced version (e.g., "espressif__led_strip" -> "led_strip")
    __component_name_without_namespace(COMPONENT "${component}" OUTPUT component_without_ns)
    if("${component_without_ns}" IN_LIST known_components)
        idf_dbg("Component '${component}' resolved to non-namespaced version '${component_without_ns}'")
        set(${ARG_OUTPUT} "${component_without_ns}" PARENT_SCOPE)
        return()
    endif()

    # 4. Check if any known component has the same name without namespace (cross-namespace matching)
    set(best_match "")
    set(best_priority -1)

    foreach(known_component IN LISTS known_components)
        __component_name_without_namespace(COMPONENT "${known_component}" OUTPUT known_without_ns)
        if("${component_without_ns}" STREQUAL "${known_without_ns}")
            # Get component priority to choose the best match
            idf_component_get_property(comp_priority "${known_component}" COMPONENT_PRIORITY)
            if(comp_priority GREATER best_priority)
                set(best_match "${known_component}")
                set(best_priority ${comp_priority})
            endif()
        endif()
    endforeach()

    if(NOT "${best_match}" STREQUAL "")
        idf_dbg("Component '${component}'
         resolved via cross-namespace matching to '${best_match}' (priority ${best_priority})")
        set(${ARG_OUTPUT} "${best_match}" PARENT_SCOPE)
        return()
    endif()

    # 5. No match found, return original name (will likely cause CMake error)
    set(${ARG_OUTPUT} "${component}" PARENT_SCOPE)
endfunction()

#[[api
.. cmakev2:function:: idf_component_include

    .. code-block:: cmake

        idf_component_include(<name>
                              [INTERFACE <variable>])

    *name[in]*

        Component name.

    *INTERFACE[out,opt]*

        Optional variable where the name of the target interface for the
        included component will be stored.

    This is a core function of the build system, responsible for including the
    specified component, as indicated by the ``name`` option, into the build
    process. It does this by evaluating the component through the
    ``add_subdirectory`` call for the component's directory. The component is
    responsible for creating its own CMake target, with the target name
    provided via the ``COMPONENT_TARGET`` variable. This target is linked to
    the component's interface target, which is managed internally by the build
    system. The component's interface target is used by other components to
    establish their dependencies.

    When the ``INTERFACE`` variable is provided, the name of the included
    component interface target will be stored in it.
#]]
function(idf_component_include name)
    set(options)
    set(one_value INTERFACE)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    # Resolve the component name to its namespaced form if needed
    idf_build_get_property(components_discovered COMPONENTS_DISCOVERED)
    __resolve_component_name(COMPONENT "${name}"
                            KNOWN_COMPONENTS "${components_discovered}"
                            OUTPUT resolved_name)

    # Check that the resolved component name is among the discovered components
    if(NOT "${resolved_name}" IN_LIST components_discovered)
        idf_die("Component '${name}' (resolved to '${resolved_name}') not found.
        Available components: '${components_discovered}'")
    endif()

    # Use the resolved name for the rest of the function
    set(name "${resolved_name}")

    # Check if the component is already included, meaning the add_subdirectory
    # has already been called for it and the component has been processed.
    idf_build_get_property(components_included COMPONENTS_INCLUDED)
    if("${name}" IN_LIST components_included)
        idf_dbg("Component '${name}' is already included.")
        # Set the parent variable ARG_INTERFACE to include the target name of
        # the component interface.
        if(DEFINED ARG_INTERFACE)
            idf_component_get_property(component_interface "${name}" COMPONENT_INTERFACE)
            set(${ARG_INTERFACE} ${component_interface} PARENT_SCOPE)
        endif()
        return()
    endif()

    idf_dbg("Including component '${name}'")

    idf_component_get_property(component_name "${name}" COMPONENT_NAME)
    idf_component_get_property(component_interface "${name}" COMPONENT_INTERFACE)
    idf_component_get_property(component_target "${name}" COMPONENT_TARGET)
    idf_component_get_property(component_directory "${name}" COMPONENT_DIR)
    idf_component_get_property(component_alias "${name}" COMPONENT_ALIAS)
    idf_component_get_property(component_lib "${name}" COMPONENT_LIB)
    idf_component_get_property(component_version "${name}" COMPONENT_VERSION)

    # The following cmakev1 backward-compatible variables are passed to the
    # component's CMakeLists.txt.
    set(COMPONENT_NAME ${component_name})
    set(COMPONENT_DIR ${component_directory})
    set(COMPONENT_ALIAS ${component_alias})
    set(COMPONENT_LIB ${component_lib})
    set(COMPONENT_VERSION ${component_version})
    set(ESP_PLATFORM 1)

    # The COMPONENT_TARGET is simply an alias for COMPONENT_LIB, which more
    # accurately reflects its purpose in the cmakev2 build system. The
    # component's responsibility is to create the target with the name
    # specified in the COMPONENT_TARGET variable.
    set(COMPONENT_TARGET ${component_target})

    # To handle potential circular dependencies among components, maintain a
    # record of currently evaluated components in the __DEPENDENCY_CHAIN. This
    # helps in detecting and reporting circular dependencies, such as
    # C1->C2->C1. In this scenario, C2 can still use the C1 interface target,
    # but C1 will only be fully evaluated after C2 has been evaluated.
    if("${component_name}" IN_LIST __DEPENDENCY_CHAIN)
        idf_dbg("Component '${name}' in circular dependency chain '${__DEPENDENCY_CHAIN}'")
        return()
    endif()

    list(APPEND __DEPENDENCY_CHAIN "${name}")
    # Evaluate the CMakeLists.txt file of the component.
    idf_build_get_property(build_dir BUILD_DIR build_dir)
    add_subdirectory("${component_directory}" "${build_dir}/esp-idf/${component_name}" EXCLUDE_FROM_ALL)

    # The component has been evaluated; remove it from the dependency chain.
    list(POP_BACK __DEPENDENCY_CHAIN)

    # Mark the component as included and add it to the COMPONENTS_INCLUDED
    # build property.  Note that COMPONENTS_INCLUDED is read again because it
    # might have been updated further down the dependency chain.
    idf_component_set_property("${component_name}" COMPONENT_INCLUDED YES)
    idf_build_get_property(components_included COMPONENTS_INCLUDED)
    list(APPEND components_included "${component_name}")
    idf_build_set_property(COMPONENTS_INCLUDED "${components_included}")

    idf_component_get_property(component_interface "${name}" COMPONENT_INTERFACE)
    if(DEFINED ARG_INTERFACE)
        set(${ARG_INTERFACE} ${component_interface} PARENT_SCOPE)
    endif()

    if(NOT TARGET "${component_target}")
        # The cmakev1 system allows component to return without creating any
        # cmake target. For instance, a component might not be supported for a
        # given IDF_TARGET. Ideally, we should fail and abort the build process
        # in such cases. However, for compatibility reasons, the absence of
        # component's target is silently ignored here.
        idf_dbg("Component '${name}' lacks a target")
        idf_component_set_property("${component_name}" COMPONENT_REAL_TARGET NOTFOUND)
        return()
    endif()

    # The component might use the COMPONENT_TARGET name as an alias for another
    # target. In this case, although unlikely, identify the real target and
    # store it in the COMPONENT_REAL_TARGET component property.
    __get_real_target(TARGET "${component_target}" OUTPUT component_real_target)
    idf_component_set_property("${component_name}" COMPONENT_REAL_TARGET "${component_real_target}")

    # Create a component interface alias, but only after the real target is
    # known, meaning the component is included. The alias has a well-defined
    # name and can be used, for example, in generator expressions without
    # needing to get the COMPONENT_ALIAS property.
    idf_component_get_property(component_alias "${name}" COMPONENT_ALIAS)
    add_library("${component_alias}" ALIAS "${component_interface}")

    # Set component target type as COMPONENT_REAL_TARGET_TYPE property.
    get_target_property(component_real_target_type "${component_real_target}" TYPE)
    idf_component_set_property("${component_name}" COMPONENT_REAL_TARGET_TYPE "${component_real_target_type}")

    # Link the target created by the component to the component interface
    # target.
    if("${component_real_target_type}" STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries("${component_interface}" INTERFACE "${component_real_target}")
    elseif("${component_real_target_type}" STREQUAL "STATIC_LIBRARY")
        idf_component_get_property(whole_archive "${component_name}" WHOLE_ARCHIVE)
        if(whole_archive)
            idf_build_get_property(linker_type LINKER_TYPE)
            if(linker_type STREQUAL "GNU")
                target_link_libraries("${component_interface}" INTERFACE
                    "-Wl,--whole-archive" "${component_real_target}"  "-Wl,--no-whole-archive")
            elseif(linker_type STREQUAL "Darwin")
                target_link_libraries("${component_interface}" INTERFACE
                    "-Wl,-force_load" "${component_real_target}")
            endif()
        else()
            target_link_libraries("${component_interface}" INTERFACE "${component_real_target}")
        endif()
    else()
        idf_die("Unsupported target type '${component_real_target_type}' in component '${component_name}'")
    endif()

    idf_component_get_property(embed_files "${component_name}" EMBED_FILES)
    foreach(file IN LISTS embed_files)
        target_add_binary_data(${COMPONENT_TARGET} "${file}" "BINARY")
    endforeach()

    idf_component_get_property(embed_txtfiles "${component_name}" EMBED_TXTFILES)
    foreach(file IN LISTS embed_txtfiles)
        target_add_binary_data(${COMPONENT_TARGET} "${file}" "TEXT")
    endforeach()

    # Inject managed dependencies if component manager is enabled
    idf_build_get_property(idf_component_manager IDF_COMPONENT_MANAGER)
    idf_component_get_property(component_format "${component_name}" COMPONENT_FORMAT)
    if(idf_component_manager EQUAL 1)
        idf_component_get_property(component_dir "${component_name}" COMPONENT_DIR)
        # Check if component has manifest for managed dependency injection
        if(EXISTS "${component_dir}/idf_component.yml")
            __inject_requirements_for_component_from_manager("${component_name}")

            # Include any managed dependencies
            idf_component_get_property(managed_requires "${component_name}" MANAGED_REQUIRES)
            idf_component_get_property(managed_priv_requires "${component_name}" MANAGED_PRIV_REQUIRES)

            foreach(dep IN LISTS managed_requires managed_priv_requires)
                if(dep)
                    idf_component_include("${dep}")
                endif()
            endforeach()

            # For cmakev1 components, automatically link managed dependencies to maintain
            # backward compatibility.
            if("${component_format}" STREQUAL "CMAKEV1")
                idf_component_get_property(component_type "${component_name}" COMPONENT_TYPE)

                # Link managed public requirements
                foreach(req IN LISTS managed_requires)
                    if(req)
                        idf_component_get_property(req_interface "${req}" COMPONENT_INTERFACE)
                        if(${component_type} STREQUAL LIBRARY)
                            target_link_libraries("${component_target}" PUBLIC "${req_interface}")
                        else()
                            target_link_libraries("${component_target}" INTERFACE "${req_interface}")
                        endif()
                    endif()
                endforeach()

                # Link managed private requirements
                foreach(req IN LISTS managed_priv_requires)
                    if(req)
                        idf_component_get_property(req_interface "${req}" COMPONENT_INTERFACE)
                        if(${component_type} STREQUAL CONFIG_ONLY)
                            continue()
                        endif()
                        target_link_libraries("${component_target}" PRIVATE "${req_interface}")
                    endif()
                endforeach()
            endif()
        endif()
    endif()

    # Components for cmakev1 use the idf_component_register call and are
    # managed in cmakev2 through a shim. This shim sets the COMPONENT_FORMAT
    # property to CMAKEV1 and applies global compilation options and
    # definitions using CMake's directory-scoped function for backward
    # compatibility. Therefore, no additional configuration is required.
    if("${component_format}" STREQUAL "CMAKEV1")
        return()
    endif()

    # The component was not handled by the idf_component_register shim,
    # consider it as a component for cmakev2.
    idf_component_set_property("${component_name}" COMPONENT_FORMAT CMAKEV2)

    # The component's target is an interface library; nothing more needs to be
    # set.
    if("${component_real_target_type}" STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    target_include_directories("${component_real_target}" BEFORE PRIVATE "${include_directories}")

    idf_build_get_property(compile_definitions COMPILE_DEFINITIONS GENERATOR_EXPRESSION)
    target_compile_definitions("${component_real_target}" PRIVATE "${compile_definitions}")

    __get_compile_options(OUTPUT compile_options)
    target_compile_options("${component_real_target}" BEFORE PRIVATE "${compile_options}")
endfunction()
