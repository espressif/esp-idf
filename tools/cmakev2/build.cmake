# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

#[[api
.. cmakev2:function:: idf_build_set_property

   .. code-block:: cmake

      idf_build_set_property(<property> <value> [APPEND])

   :property[in]: Property name.
   :value[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

   Set the value of the specified property related to the ESP-IDF build. The
   property is also added to the internal list of build properties if it isn't
   already there.
#]]
function(idf_build_set_property property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(append)
    if(ARG_APPEND)
        set(append APPEND)
    endif()

    __set_property(TARGET idf_build_properties
                   PROPERTY "${property}"
                   PROPERTIES BUILD_PROPERTIES
                   VALUE "${value}"
                   ${append})
endfunction()

#[[api
.. cmakev2:function:: idf_build_get_property

   .. code-block:: cmake

      idf_build_get_property(<var> <property> [GENERATOR_EXPRESSION])

   :variable[out]: Variable to store the value in.
   :property[in]: Property name to get the value of.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

   Get the value of the specified property related to the ESP-IDF build.
#]]
function(idf_build_get_property variable property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if("${property}" STREQUAL BUILD_COMPONENTS)
        idf_die("Build property 'BUILD_COMPONENTS' is not supported")
    endif()

    set(genexpr)
    if(ARG_GENERATOR_EXPRESSION)
        set(genexpr GENERATOR_EXPRESSION)
    endif()

    __get_property(TARGET idf_build_properties
                   PROPERTY "${property}"
                   OUTPUT value
                   ${genexpr})
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
   __dump_build_properties()

   Dump all build properties.
#]]
function(__dump_build_properties)
    idf_build_get_property(properties BUILD_PROPERTIES)
    idf_msg("build properties: ${properties}")
    foreach(property IN LISTS properties)
        idf_build_get_property(value ${property})
        idf_msg("   ${property}: ${value}")
    endforeach()
endfunction()

#[[
   __get_library_interface_or_die(LIBRARY <library>
                                  OUTPUT <variable>)

   :LIBRARY[in]: Library interface or alias.
   :OUTPUT[out]: Output variable to store the library interface.

   Verify that "LIBRARY" is a known interface created by ``idf_build_library``
   or its alias. If it is, return the library interface; otherwise, terminate
   the build process.
#]]
function(__get_library_interface_or_die)
    set(options)
    set(one_value LIBRARY OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_LIBRARY)
        idf_die("LIBRARY option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    __get_real_target(TARGET ${ARG_LIBRARY} OUTPUT library_interface)
    idf_build_get_property(library_interfaces LIBRARY_INTERFACES)

    if(NOT "${library_interface}" IN_LIST library_interfaces)
        idf_die("Library interface '${ARG_LIBRARY}' does not exist")
    endif()
    set(${ARG_OUTPUT} ${library_interface} PARENT_SCOPE)
endfunction()

#[[api
.. cmakev2:function:: idf_library_set_property

   .. code-block:: cmake

      idf_library_set_property(<library> <property> <value> [APPEND])

   :library[in]: Library interface target or alias.
   :property[in]: Property name.
   :value[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

   Set the value of the specified library property. The property is also
   added to the internal list of library properties if it isn't already
   there.
#]]
function(idf_library_set_property library property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(append)
    if(ARG_APPEND)
        set(append APPEND)
    endif()

    __get_library_interface_or_die(LIBRARY "${library}" OUTPUT library_interface)
    __set_property(TARGET "${library_interface}"
                   PROPERTY "${property}"
                   PROPERTIES LIBRARY_PROPERTIES
                   VALUE "${value}"
                   ${append})
endfunction()

#[[api
.. cmakev2:function:: idf_library_get_property

   .. code-block:: cmake

      idf_library_get_property(<variable> <library> <property> [GENERATOR_EXPRESSION])

   :variable[out]: Variable to store the value in.
   :library[in]: Library interface target or alias.
   :property[in]: Property name to get the value of.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

   Retrieve the value of the specified library property.
#]]
function(idf_library_get_property variable library property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(genexpr)
    if(ARG_GENERATOR_EXPRESSION)
        set(genexpr GENERATOR_EXPRESSION)
    endif()

    __get_library_interface_or_die(LIBRARY "${library}" OUTPUT library_interface)
    __get_property(TARGET "${library_interface}"
                   PROPERTY "${property}"
                   OUTPUT value
                   ${genexpr})
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
   __dump_library_properties(<libraries>)

   :libraries: List of library interfaces whose properties should be displayed.

   Dump all properties for the libraries listed in ``<libraries>``.
#]]
function(__dump_library_properties libraries)
    foreach(library IN LISTS libraries)
        idf_library_get_property(properties "${library}" LIBRARY_PROPERTIES)
        idf_msg("library '${library}' properties: ${properties}")
        foreach(property IN LISTS properties)
            idf_library_get_property(value "${library}" "${property}")
            idf_msg("   ${property}: ${value}")
        endforeach()
    endforeach()
endfunction()

#[[api
.. cmakev2:function:: idf_build_library

   .. code-block:: cmake

       idf_build_library(INTERFACE <interface>
                         [COMPONENTS <component>...])

   :INTERFACE[in,out]: Name of the library interface to be created.
   :COMPONENTS[in,opt]: List of component names to add to the library.

   Create a new library interface target with the name specified in the
   ``INTERFACE`` option and link component targets to it based on the component
   names provided in the ``COMPONENTS`` option. If ``COMPONENTS`` option is not
   set, link component targets of all discovered components.

   List of library properties

   :LIBRARY_COMPONENTS: List of component as specified by the ``COMPONENTS``
                        option.
   :LIBRARY_COMPONENTS_LINKED: List of components linked to the library based
                               on recursive evaluations of the INTERFACE_LINK_LIBRARIES
                               and LINK_LIBRARIES target properties.
#]]
function(idf_build_library)
    set(options)
    set(one_value INTERFACE)
    set(multi_value COMPONENTS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    idf_build_get_property(project_initialized __PROJECT_INITIALIZED)
    if(NOT project_initialized)
        idf_die("The IDF project is not initialized. The 'idf_project_init()' must be called first.")
    endif()

    if(NOT DEFINED ARG_INTERFACE)
        idf_die("INTERFACE option is required")
    endif()

    if(NOT DEFINED ARG_COMPONENTS)
        # The library should include all discovered components.
        idf_build_get_property(component_names COMPONENTS_DISCOVERED)
        set(ARG_COMPONENTS "${component_names}")
    endif()

    # Create library interface.
    add_library("${ARG_INTERFACE}" INTERFACE)
    idf_build_set_property(LIBRARY_INTERFACES "${ARG_INTERFACE}" APPEND)
    idf_library_set_property("${ARG_INTERFACE}" LIBRARY_COMPONENTS "${ARG_COMPONENTS}")

    # Add global include directories, such as the directory containing
    # sdkconfig, to the library's include directories.
    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    target_include_directories("${ARG_INTERFACE}" INTERFACE "${include_directories}")

    # Add link options.
    idf_build_get_property(link_options LINK_OPTIONS)
    target_link_options(${ARG_INTERFACE} INTERFACE "${link_options}")

    # Include the requested components and link their interface targets to the
    # library.
    foreach(component_name IN LISTS ARG_COMPONENTS)
        idf_component_include("${component_name}")
        idf_component_get_property(component_interface "${component_name}" COMPONENT_INTERFACE)
        target_link_libraries("${ARG_INTERFACE}" INTERFACE "${component_interface}")
    endforeach()

    # Get all targets transitively linked to the library interface target.
    __get_target_dependencies(TARGET "${ARG_INTERFACE}" OUTPUT dependencies)

    # Identify the components linked to the library by looking at all targets
    # that are transitively linked to the library and mapping these targets to
    # components. Store the linked component interfaces in
    # LIBRARY_COMPONENTS_LINKED property.
    set(component_interfaces_linked)
    foreach(dep IN LISTS dependencies)
        __get_component_interface(COMPONENT "${dep}" OUTPUT component_interface)
        if(NOT component_interface)
            continue()
        endif()
        if("${component_interface}" IN_LIST component_interfaces_linked)
            continue()
        endif()

        list(APPEND component_interfaces_linked "${component_interface}")
        idf_component_get_property(component_name "${component_interface}" COMPONENT_NAME)
        idf_library_set_property("${ARG_INTERFACE}" LIBRARY_COMPONENTS_LINKED "${component_name}" APPEND)
    endforeach()

    # Collect linker fragment files from all components linked to the library
    # interface and store them in the __LDGEN_FRAGMENT_FILES files. This
    # property is used by ldgen to generate template-based linker scripts.
    foreach(component_interface IN LISTS component_interfaces_linked)
        idf_component_get_property(component_ldfragments "${component_interface}" LDFRAGMENTS)
        idf_component_get_property(component_directory "${component_interface}" COMPONENT_DIR)
        __get_absolute_paths(PATHS "${component_ldfragments}"
                             BASE_DIR "${component_directory}"
                             OUTPUT ldfragments)
        idf_library_set_property("${ARG_INTERFACE}" __LDGEN_FRAGMENT_FILES "${ldfragments}" APPEND)
    endforeach()

    # Collect archive files from all targets linked to the library interface
    # and store them in the __LDGEN_DEPENDS and __LDGEN_LIBRARIES library
    # properties. These properties are used by ldgen to generate linker scripts
    # from templates. The __LDGEN_LIBRARIES property contains a list of
    # TARGET_FILE generator expressions for archive files.
    foreach(dep IN LISTS dependencies)
        if(NOT TARGET "${dep}")
            continue()
        endif()

        get_target_property(type "${dep}" TYPE)
        if("${type}" STREQUAL "INTERFACE_LIBRARY")
            continue()
        endif()

        idf_library_get_property(ldgen_depends "${ARG_INTERFACE}" __LDGEN_DEPENDS)
        if(NOT "${dep}" IN_LIST ldgen_depends)
            idf_library_set_property("${ARG_INTERFACE}" __LDGEN_LIBRARIES "$<TARGET_FILE:${dep}>" APPEND)
            idf_library_set_property("${ARG_INTERFACE}" __LDGEN_DEPENDS ${dep} APPEND)
        endif()
    endforeach()

    # Create a sanitized library interface name that can be used as a suffix
    # for files and targets specific to the library.
    string(REGEX REPLACE "[^A-Za-z0-9_]" "_" suffix "_${ARG_INTERFACE}")

    foreach(component_interface IN LISTS component_interfaces_linked)
        # Generate linker scripts from templates.
        # LINKER_SCRIPTS_TEMPLATE and LINKER_SCRIPTS_GENERATED are parallel
        # lists. The first holds the template linker script path, and the
        # second holds the generated linker script path.
        idf_component_get_property(template_scripts "${component_interface}" LINKER_SCRIPTS_TEMPLATE)
        idf_component_get_property(generated_scripts "${component_interface}" LINKER_SCRIPTS_GENERATED)

        set(scripts)
        foreach(template script IN ZIP_LISTS template_scripts generated_scripts)
            set(script "${script}${suffix}")
            __ldgen_process_template(LIBRARY "${ARG_INTERFACE}"
                                     TEMPLATE "${template}"
                                     SUFFIX "${suffix}"
                                     OUTPUT "${script}")
            list(APPEND scripts "${script}")
            # Add a custom target for the generated script and include it as a
            # dependency for the library interface to ensure the script is
            # generated before linking.
            get_filename_component(basename "${script}" NAME)
            string(REGEX REPLACE "[^A-Za-z0-9_]" "_" basename "${basename}")
            add_custom_target(__ldgen_output_${basename} DEPENDS "${script}")
            add_dependencies("${ARG_INTERFACE}" __ldgen_output_${basename})
        endforeach()

        # Add linker scripts.
        idf_component_get_property(scripts_static "${component_interface}" LINKER_SCRIPTS_STATIC)
        list(PREPEND scripts "${scripts_static}")
        foreach(script IN LISTS scripts)
            get_filename_component(script_dir "${script}" DIRECTORY)
            get_filename_component(script_name "${script}" NAME)
            # Add linker script directory to the linker search path.
            target_link_directories("${ARG_INTERFACE}" INTERFACE "${script_dir}")
            # Add linker script to link. Regarding the usage of SHELL, see
            # https://cmake.org/cmake/help/latest/command/target_link_options.html#option-de-duplication
            target_link_options("${ARG_INTERFACE}" INTERFACE "SHELL:-T ${script_name}")
            # Add the linker script as a dependency to ensure the executable is
            # re-linked if the script changes.
            set_property(TARGET "${ARG_INTERFACE}" APPEND PROPERTY INTERFACE_LINK_DEPENDS "${script}")
        endforeach()
    endforeach()
endfunction()
