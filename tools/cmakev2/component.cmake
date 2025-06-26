# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)
include(build)

#[[api
.. cmakev2:function:: idf_component_set_property

   .. code-block:: cmake

      idf_component_set_property(<property> <value> [APPEND])

   :property[in]: Property name.
   :value[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

   Set the value of the specified component property. The property is also
   added to the internal list of component properties if it isn't already
   there.
#]]
function(idf_component_set_property component property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    __get_component_interface_or_die(COMPONENT "${component}" OUTPUT component_interface)
    get_property(properties TARGET ${component_interface} PROPERTY COMPONENT_PROPERTIES)
    if(NOT property IN_LIST properties)
        list(APPEND properties "${property}")
        set_property(TARGET ${component_interface} PROPERTY COMPONENT_PROPERTIES "${properties}")
    endif()
    if(ARG_APPEND)
        set_property(TARGET ${component_interface} APPEND PROPERTY ${property} "${value}")
    else()
        set_property(TARGET ${component_interface} PROPERTY ${property} "${value}")
    endif()
endfunction()

#[[api
.. cmakev2:function:: idf_component_get_property

   .. code-block:: cmake

      idf_component_get_property(<variable> <property> [GENERATOR_EXPRESSION])

   :variable[out]: Variable to store the value in.
   :property[in]: Property name to get the value of.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

   Retrieve the value of the specified component property.
#]]
function(idf_component_get_property variable component property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    __get_component_interface_or_die(COMPONENT "${component}" OUTPUT component_interface)
    if(ARG_GENERATOR_EXPRESSION)
        set(value "$<TARGET_PROPERTY:${component_interface},${property}>")
    else()
        get_property(value TARGET ${component_interface} PROPERTY ${property})
    endif()
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
   __get_component_paths(PATHS <path>...
                         [EXCLUDE_PATHS <path>...]
                         [SOURCE <source>]
                         [CHECK]
                         OUTPUT <var>)

   :PATHS[in]: List of paths to search for component directories.
   :EXCLUDE_PATHS[in,opt]: Optional list of paths to exclude from the search of
                           component directories.
   :SOURCE[in,opt]: Source of the ``PATHS``. If provided, it will be included
                    in the error message when ``CHECK`` is specified.
   :CHECK[in,opt]: Verify whether the paths listed in "PATHS" exist. If any
                   path is missing, abort the build process.
   :OUTPUT[out]: Output variable to store the list of found component
                 directories.

   Search for component directories in the specified ``PATHS``, excluding those
   in ``EXCLUDE_PATHS``, and store the list of absolute component paths in the
   ``OUTPUT`` variable. If ``CHECK`` is specified, ensure that the paths listed
   in ``PATHS`` exist, and stop the build process if they do not.
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

    set(include_paths "")
    set(exclude_paths "")
    __get_absolute_paths(PATHS "${ARG_PATHS}" OUTPUT include_paths)
    if(DEFINED ARG_EXCLUDE_PATHS)
        __get_absolute_paths(PATHS "${ARG_EXCLUDE_PATHS}" OUTPUT exclude_paths)
    endif()

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

   :COMPONENT[int]: Component name, target, target alias or interface.
   :OUTPUT[out]: Output variable to store the component interface.

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
    if("${ARG_COMPONENT}" IN_LIST component_names)
        # The component name is among the discovered components, and the
        # component interface is simply the component name with a prefix.
        set(component_interface "${component_prefix}_${ARG_COMPONENT}")
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

   :COMPONENT[int]: Component name, target, target alias or interface.
   :OUTPUT[out]: Output variable to store the component interface.

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

   :SOURCE[in]: String identifying the component source.
   :OUTPUT[out]: Output variable to store the component priority.

   Return the priority number of a component, where a higher number indicates a
   higher priority, based on the given ``source`` string. If the ``source`` is
   not valid, return ``NOTFOUND``.
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

   :DIRECTORY[in]: ``<path>`` where the component is located.
   :PREFIX[in]: Prefix for component target names.
   :SOURCE[in]: String identifying the component source.

   Initialize the component by creating a component interface target, allowing
   properties to be attached to it and also add initial component properties.
   At this stage, the component is not included in the build. The actual
   component target, named as specified in the `COMPONENT_LIB` property, is
   created later when ``add_subdirectory`` is called for the component's
   directory based on the project or other component requirements.

   Components are identified by the directory name they reside in. This means
   that components with the same name might exist in different directory paths.
   In such cases, the component with the higher priority is used. Priority is
   determined by the component's source, as defined in
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
    add_library("${component_alias}" ALIAS "${component_interface}")

    idf_build_set_property(COMPONENTS_DISCOVERED "${component_name}" APPEND)
    idf_build_set_property(COMPONENT_INTERFACES "${component_interface}" APPEND)

    idf_component_set_property("${component_name}" COMPONENT_LIB "${component_target}")
    idf_component_set_property("${component_name}" COMPONENT_NAME "${component_name}")
    idf_component_set_property("${component_name}" COMPONENT_DIR "${component_directory}")
    idf_component_set_property("${component_name}" COMPONENT_ALIAS "${component_alias}")
    idf_component_set_property("${component_name}" COMPONENT_SOURCE "${component_source}")
    idf_component_set_property("${component_name}" COMPONENT_INTERFACE "${component_interface}")
    idf_component_set_property("${component_name}" COMPONENT_PRIORITY ${component_priority})
endfunction()

#[[
   __dump_component_properties(<components>)

   :components: List of components whose properties should be displayed.

   Dump all properties for the components listed in ``<components>``.
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
