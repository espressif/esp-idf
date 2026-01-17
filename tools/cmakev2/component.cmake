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

#[[
.. cmakev2:function:: __idf_component_get_property_unchecked

    .. code-block:: cmake

        __idf_component_get_property_unchecked(<variable> <component_interface> <property>)

    *variable[out]*

        Variable to store the value in.

    *component_interface[in]*

        Component interface target.

    *property[in]*

        Property name to get the value of.

    Retrieve the value of the specified component property quickly. The
    ``idf_component_get_property`` function performs various checks to identify
    the component interface target, which keeps the component properties and
    accepts a component name, target, target alias, or interface. While
    ``idf_component_get_property`` uses a cache to identify the component
    interface relatively quickly, it is still much slower compared to the raw
    ``get_property``. This function should only be used if the component
    interface is already known. It skips all the checks, so it must be used
    carefully.
#]]
function(__idf_component_get_property_unchecked variable component_interface property)
    get_property(value TARGET ${component_interface} PROPERTY ${property})
    set(${variable} ${value} PARENT_SCOPE)
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
    __get_component_cached_interface(<variable> <component>)

    *variable[out]*

        Variable to store the cached component target name, or left empty if
        not found.

    *component[in]*

        Component name to search for in the cache.

    Get the component target interface for a given ``component`` name. The cache
    maps multiple component names to the component interface target. The name
    can be a component name with or without a namespace, target, interface, or
    alias.
#]]
function(__get_component_cached_interface variable component)
    __get_property(TARGET __idf_component_interface_cache
                   PROPERTY "${component}"
                   OUTPUT interface)
    set(${variable} "${interface}" PARENT_SCOPE)
endfunction()

#[[
    __set_component_cached_interface(<component> <interface>)

    *component[in]*

        Component name with or without a namespace, target, interface, or
        alias.

    *interface[in]*

        Component interface target.

    Set the name mapping for the ``component`` to the ``interface`` target in
    the cache.
#]]
function(__set_component_cached_interface component interface)
    __set_property(TARGET __idf_component_interface_cache
                   PROPERTY "${component}"
                   PROPERTIES INTERFACE_CACHE_PROPERTIES
                   VALUE "${interface}")
endfunction()

#[[
    __dump_component_interface_cache()

    Dump content of internal component interface cache.
#]]
function(__dump_component_interface_cache)
    __get_property(TARGET __idf_component_interface_cache
                   PROPERTY INTERFACE_CACHE_PROPERTIES
                   OUTPUT properties)
    idf_msg("component interface cache: ${properties}")
    foreach(property IN LISTS properties)
        __get_property(TARGET __idf_component_interface_cache
                       PROPERTY "${property}"
                       OUTPUT value)
        idf_msg("   ${property}: ${value}")
    endforeach()
endfunction()

#[[
    __init_component_interface_cache(<name> <interface> <alias> <target> <priority>)

    *name[in]*

        Component name. The component name is the same as the component
        directory name.

    *interface[in]*

        Component interface.

    *alias[in]*

        Component alias.

    *target[in]*

        Component target.

    *priority[in]*

        Component priority.

    Initialize the component interface cache for given component by adding
    mappings from the component name, interface, alias, and target to the
    component interface.  The component interface cache is used by the
    __get_component_interface function to quickly get a component interface
    target based on the given component name, target, and alias. If the
    component name contains a namespace prefix, the short name of the component
    is also added to the cache. For example, for "espressif__led_strip," a
    short name "led_strip" is added as a mapping to the "espressif__led_strip"
    component target in the component interface cache. Since the short name may
    cause ambiguity, component preference is used to determine which component
    interface the short name should point to.
#]]
function(__init_component_interface_cache name interface alias target priority)
    __set_component_cached_interface("${name}" "${interface}")
    __set_component_cached_interface("${interface}" "${interface}")
    __set_component_cached_interface("${alias}" "${interface}")
    __set_component_cached_interface("${target}" "${interface}")

    string(REGEX REPLACE ".*__" "" short_name "${name}")
    if("${short_name}" STREQUAL "${name}")
        # The component name does not include a namespace, so there is nothing
        # further to set.
        return()
    endif()

    __get_component_cached_interface(short_name_interface "${short_name}")
    if(NOT short_name_interface)
        # The component interface cache does not yet contain an entry for a
        # component name without a namespace, so add it as the name for the
        # given component interface.
        idf_component_set_property("${name}" COMPONENT_SHORT_NAME "${short_name}")
        __set_component_cached_interface("${short_name}" "${interface}")
        return()
    endif()

    # We have two components with the same short name but different namespaces.
    # For example, there is "espressif__led_strip" and "my__led_strip", and the
    # cache already contains a mapping of "led_strip" to
    # "espressif__led_strip."

    idf_component_get_property(short_name_priority "${short_name}" COMPONENT_PRIORITY)

    if(${priority} GREATER ${short_name_priority})
        # The short component name in the cache currently refers to a component
        # with lower priority. Update the short name to point to the component
        # with higher priority.
        idf_component_set_property("${short_name}" COMPONENT_SHORT_NAME "")
        idf_component_set_property("${name}" COMPONENT_SHORT_NAME "${short_name}")
        __set_component_cached_interface("${short_name}" "${interface}")
    elseif(${priority} EQUAL ${short_name_priority})
        # The short component name refers to a component with the same
        # priority. Resolve this ambiguity by completely removing the short
        # component name from the cache.
        idf_component_set_property("${short_name}" COMPONENT_SHORT_NAME "")
        __set_component_cached_interface("${short_name}" "")
    endif()
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
    which could be a component name with or without a namespace, target, target
    alias, or interface. Return the component interface target, or NOTFOUND if
    the interface cannot be located.

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

    set(component_interface NOTFOUND)

    __get_component_cached_interface(component_interface_cached "${ARG_COMPONENT}")

    if(component_interface_cached)
        set(component_interface "${component_interface_cached}")
    else()
        # Check if the COMPONENT argument is not an alias. If it is an alias,
        # resolve it to the real target and search the interface cache again
        # using the real target.
        __get_real_target(TARGET "${ARG_COMPONENT}" OUTPUT real_target)
        if(real_target)
            __get_component_cached_interface(component_interface_cached "${real_target}")
            if(component_interface_cached)
                set(component_interface "${component_interface_cached}")
                # The component is referred to by a new alias, so it should be
                # added to the component interface cache.
                __set_component_cached_interface("${ARG_COMPONENT}" "${component_interface_cached}")
            endif()
        endif()
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
        idf_build_get_property(components_discovered COMPONENTS_DISCOVERED)
        idf_die("Failed to resolve component '${ARG_COMPONENT}'.\n"
                "Available components: ${components_discovered}")
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
    if(existing_component_interface)
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

            # Store the overridden component directory (original component directory) for compatibility with cmakev1
            idf_component_set_property("${component_name}" COMPONENT_OVERRIDEN_DIR "${existing_component_directory}")

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

    idf_build_get_property(build_dir BUILD_DIR)
    set(component_build_dir "${build_dir}/esp-idf/${component_name}")

    # Interface target is used to attach all component properties and is also
    # used when the component is linked to other targets.
    add_library("${component_interface}" INTERFACE)

    # Initialize the component interface cache for the included component. This
    # establishes mappings between the component name (with or without
    # namespace), target, interface, and alias to the component interface.  The
    # component interface cache is utilized in the __get_component_interface
    # method to retrieve a component interface, for example, based on the
    # component name.
    __init_component_interface_cache("${component_name}"
                                     "${component_interface}"
                                     "${component_alias}"
                                     "${component_target}"
                                     "${component_priority}")

    idf_build_set_property(COMPONENTS_DISCOVERED "${component_name}" APPEND)
    idf_build_set_property(COMPONENT_INTERFACES "${component_interface}" APPEND)

    idf_component_set_property("${component_name}" COMPONENT_LIB "${component_target}")
    idf_component_set_property("${component_name}" COMPONENT_TARGET "${component_target}")
    idf_component_set_property("${component_name}" COMPONENT_NAME "${component_name}")
    idf_component_set_property("${component_name}" COMPONENT_DIR "${component_directory}")
    idf_component_set_property("${component_name}" COMPONENT_BUILD_DIR "${component_build_dir}")
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
    __set_component_cmakev1_properties(<component>)

    *component[in]*

        Component name.

    To maintain backward compatibility with cmakev1 components, add the
    component properties: INCLUDE_DIRS, PRIV_INCLUDE_DIRS, REQUIRES,
    PRIV_REQUIRES, and SRCS. This is achieved by examining the real target
    properties of the component and reconstructing the variables that are
    otherwise explicitly provided with idf_component_register for cmakev1
    components.
#]]
function(__set_component_cmakev1_properties component_name)
    idf_component_get_property(component_real_target "${component_name}" COMPONENT_REAL_TARGET)
    idf_component_get_property(component_dir "${component_name}" COMPONENT_DIR)

    if(NOT component_real_target)
        return()
    endif()

    # Set SRCS and COMPONENT_TYPE properties
    get_target_property(component_sources "${component_real_target}" SOURCES)
    if(component_sources)
        __get_absolute_paths(PATHS "${component_sources}" BASE_DIR "${component_dir}" OUTPUT sources)
        idf_component_set_property("${component_name}" SRCS "${sources}")
        idf_component_set_property("${component_name}" COMPONENT_TYPE LIBRARY)
    else()
        idf_component_set_property("${component_name}" COMPONENT_TYPE CONFIG_ONLY)
    endif()

    # Set INCLUDE_DIRS and PRIV_INCLUDE_DIRS properties
    get_target_property(include_dirs "${component_real_target}" INCLUDE_DIRECTORIES)
    if(NOT include_dirs)
        # get_target_property may return include_dirs-NOTFOUND
        set(include_dirs "")
    endif()
    __remove_genex(include_dirs)

    get_target_property(interface_include_dirs "${component_real_target}" INTERFACE_INCLUDE_DIRECTORIES)
    if(NOT interface_include_dirs)
        # get_target_property may return interface_include_dirs-NOTFOUND
        set(interface_include_dirs "")
    endif()
    __remove_genex(interface_include_dirs)

    __list_intersection(interface_include_dirs include_dirs public_include_dirs_abs)
    __list_difference(include_dirs public_include_dirs_abs priv_include_dirs_abs)

    __get_relative_paths(PATHS "${public_include_dirs_abs}" BASE_DIR "${component_dir}" OUTPUT public_include_dirs)
    __get_relative_paths(PATHS "${priv_include_dirs_abs}" BASE_DIR "${component_dir}" OUTPUT priv_include_dirs)

    idf_component_set_property(${component_name} INCLUDE_DIRS "${public_include_dirs}")
    idf_component_set_property(${component_name} PRIV_INCLUDE_DIRS "${priv_include_dirs}")

    # Set REQUIRES and PRIV_REQUIRES properties
    get_target_property(link_libraries ${component_real_target} LINK_LIBRARIES)
    set(link_components "")
    foreach(lib IN LISTS link_libraries)
        __get_component_interface(COMPONENT "${lib}" OUTPUT req_interface)
        if(req_interface)
            idf_component_get_property(req_name "${req_interface}" COMPONENT_NAME)
            list(APPEND link_components "${req_name}")
        endif()
    endforeach()

    get_target_property(interface_link_libraries ${component_real_target} INTERFACE_LINK_LIBRARIES)
    set(interface_link_components "")
    foreach(lib IN LISTS interface_link_libraries)
        __get_component_interface(COMPONENT "${lib}" OUTPUT req_interface)
        if(req_interface)
            idf_component_get_property(req_name "${req_interface}" COMPONENT_NAME)
            list(APPEND interface_link_components "${req_name}")
        endif()
    endforeach()

    __list_intersection(interface_link_components link_components requires)
    __list_difference(link_components requires priv_requires)

    idf_component_set_property(${component_name} REQUIRES "${requires}")
    idf_component_set_property(${component_name} PRIV_REQUIRES "${priv_requires}")
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

    __get_component_interface_or_die(COMPONENT "${name}"
                                     OUTPUT component_interface)

    # Check if the component is already included, meaning the add_subdirectory
    # has already been called for it and the component has been processed.  If
    # the component is already included, it will have the COMPONENT_INCLUDED
    # set to YES.
    idf_component_get_property(component_included "${name}" COMPONENT_INCLUDED)
    if(component_included)
        idf_dbg("Component '${name}' is already included.")
        # Set the parent variable ARG_INTERFACE to include the target name of
        # the component interface.
        if(DEFINED ARG_INTERFACE)
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
    idf_component_get_property(component_build_dir "${component_name}" COMPONENT_BUILD_DIR)
    add_subdirectory("${component_directory}" "${component_build_dir}")

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
                target_link_options("${component_interface}" INTERFACE
                    "SHELL:-Wl,--whole-archive $<TARGET_FILE:${component_real_target}> -Wl,--no-whole-archive")
                target_link_libraries("${component_interface}" INTERFACE "${component_real_target}")
            elseif(linker_type STREQUAL "Darwin")
                target_link_options("${component_interface}" INTERFACE
                    "SHELL:-Wl,-force_load $<TARGET_FILE:${component_real_target}>")
                target_link_libraries("${component_interface}" INTERFACE "${component_real_target}")
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

    # Add INCLUDE_DIRS, PRIV_INCLUDE_DIRS, REQUIRES, PRIV_REQUIRES and SRCS
    # component properties.
    __set_component_cmakev1_properties("${component_name}")

    # The component's target is an interface library; nothing more needs to be
    # set.
    if("${component_real_target_type}" STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    # Set the component archive file name.
    set_target_properties(${component_real_target} PROPERTIES OUTPUT_NAME ${component_name} LINKER_LANGUAGE C)

    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    target_include_directories("${component_real_target}" BEFORE PRIVATE "${include_directories}")

    idf_build_get_property(compile_definitions COMPILE_DEFINITIONS GENERATOR_EXPRESSION)
    target_compile_definitions("${component_real_target}" PRIVATE "${compile_definitions}")

    __get_compile_options(OUTPUT compile_options)
    target_compile_options("${component_real_target}" BEFORE PRIVATE "${compile_options}")
endfunction()
