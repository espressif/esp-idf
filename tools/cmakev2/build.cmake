# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(component_validation)

#[[api
.. cmakev2:function:: idf_build_set_property

    .. code-block:: cmake

        idf_build_set_property(<property> <value> [APPEND])

    *property[in]*

        Property name.

    *value[in]*

        Property value.

    *APPEND*

        Append the value to the property's current value instead of replacing
        it.

    Set the value of the specified property related to the ESP-IDF build. The
    property is also added to the internal list of build properties if it isn't
    already there.
#]]
function(idf_build_set_property property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if("${property}" STREQUAL MINIMAL_BUILD)
        idf_warn("Build property 'MINIMAL_BUILD' is obsolete and will be ignored")
    endif()

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

    *variable[out]*

        Variable to store the value in.

    *property[in]*

        Property name to get the value of.

    *GENERATOR_EXPRESSION[in]*

        Obtain the generator expression for the property rather than the actual
        value.

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

    *LIBRARY[in]*

        Library interface or alias.

    *OUTPUT[out]*

        Output variable to store the library interface.

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

#[[
.. cmakev2:function:: idf_library_set_property

    .. code-block:: cmake

        idf_library_set_property(<library> <property> <value> [APPEND])

    *library[in]*

        Library interface target or alias.

    *property[in]*

        Property name.

    *value[in]*

        Property value.

    *APPEND*

        Append the value to the property's current value instead of replacing
        it.

    Set the value of the specified library property. The property is also added
    to the internal list of library properties if it isn't already there.
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

#[[
.. cmakev2:function:: idf_library_get_property

    .. code-block:: cmake

        idf_library_get_property(<variable> <library> <property> [GENERATOR_EXPRESSION])

    *variable[out]*

        Variable to store the value in.

    *library[in]*

        Library interface target or alias.

    *property[in]*

        Property name to get the value of.

    *GENERATOR_EXPRESSION*

        Obtain the generator expression for the property rather than the actual
        value.

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

    *libraries*

        List of library interfaces whose properties should be displayed.

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

#[[
.. cmakev2:function:: idf_build_library

    .. code-block:: cmake

        idf_build_library(<library>
                          [COMPONENTS <component>...])

    *library[in]*

        Name of the library interface to be created.

    *COMPONENTS[in,opt]*

        List of component names to add to the library.

    Create a new library interface target with the name specified in the
    ``library`` option and link component targets to it based on the component
    names provided in the ``COMPONENTS`` option. If ``COMPONENTS`` option is
    not set, link component targets of all discovered components.

    List of library properties

    LIBRARY_COMPONENTS
        List of component as specified by the ``COMPONENTS`` option.

    LIBRARY_COMPONENTS_LINKED
        List of component names linked to the library based on recursive evaluations
        of the INTERFACE_LINK_LIBRARIES and LINK_LIBRARIES target properties.

    LIBRARY_COMPONENT_INTERFACES_LINKED
        List of component interfaces linked to the library based on recursive
        evaluations of the INTERFACE_LINK_LIBRARIES and LINK_LIBRARIES target
        properties.
#]]
function(idf_build_library library)
    set(options)
    set(one_value)
    set(multi_value COMPONENTS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    idf_build_get_property(project_initialized __PROJECT_INITIALIZED)
    if(NOT project_initialized)
        idf_die("The IDF project is not initialized. The 'idf_project_init()' must be called first.")
    endif()

    if(NOT DEFINED ARG_COMPONENTS)
        # The library should include all discovered components.
        idf_build_get_property(component_names COMPONENTS_DISCOVERED)
        set(ARG_COMPONENTS "${component_names}")
    endif()

    # Create library interface.
    add_library("${library}" INTERFACE)
    idf_build_set_property(LIBRARY_INTERFACES "${library}" APPEND)
    idf_library_set_property("${library}" LIBRARY_COMPONENTS "${ARG_COMPONENTS}")

    # Add global include directories, such as the directory containing
    # sdkconfig, to the library's include directories.
    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    target_include_directories("${library}" INTERFACE "${include_directories}")

    # Add link options.
    idf_build_get_property(link_options LINK_OPTIONS)
    target_link_options(${library} INTERFACE "${link_options}")

    # Include the requested components and link their interface targets to the
    # library.
    foreach(component_name IN LISTS ARG_COMPONENTS)
        idf_component_include("${component_name}")
        idf_component_get_property(component_interface "${component_name}" COMPONENT_INTERFACE)
        target_link_libraries("${library}" INTERFACE "${component_interface}")
    endforeach()

    # Get all targets transitively linked to the library interface target.
    __get_target_dependencies(TARGET "${library}" OUTPUT dependencies)

    # Identify the components linked to the library by looking at all targets
    # that are transitively linked to the library and mapping these targets to
    # components. Store the linked component name in LIBRARY_COMPONENTS_LINKED
    # and component interface in LIBRARY_COMPONENT_INTERFACES_LINKED property.
    foreach(dep IN LISTS dependencies)
        __get_component_interface(COMPONENT "${dep}" OUTPUT component_interface)
        if(NOT component_interface)
            continue()
        endif()
        idf_library_get_property(component_interfaces_linked "${library}" LIBRARY_COMPONENT_INTERFACES_LINKED)
        if("${component_interface}" IN_LIST component_interfaces_linked)
            continue()
        endif()

        idf_component_get_property(component_name "${component_interface}" COMPONENT_NAME)
        idf_library_set_property("${library}" LIBRARY_COMPONENTS_LINKED "${component_name}" APPEND)
        idf_library_set_property("${library}" LIBRARY_COMPONENT_INTERFACES_LINKED "${component_interface}" APPEND)
    endforeach()

    # Collect linker fragment files from all components linked to the library
    # interface and store them in the __LDGEN_FRAGMENT_FILES files. This
    # property is used by ldgen to generate template-based linker scripts.
    idf_library_get_property(component_interfaces_linked "${library}" LIBRARY_COMPONENT_INTERFACES_LINKED)
    foreach(component_interface IN LISTS component_interfaces_linked)
        idf_component_get_property(component_ldfragments "${component_interface}" LDFRAGMENTS)
        idf_component_get_property(component_directory "${component_interface}" COMPONENT_DIR)
        __get_absolute_paths(PATHS "${component_ldfragments}"
                             BASE_DIR "${component_directory}"
                             OUTPUT ldfragments)
        idf_library_set_property("${library}" __LDGEN_FRAGMENT_FILES "${ldfragments}" APPEND)
    endforeach()

    # Collect the filenames of project component archives, which are considered
    # mutable, in the __LDGEN_MUTABLE_LIBS library property. These libraries
    # are placed by ldgen into a separate location in the linker script,
    # enabling the fast reflashing feature.
    idf_library_get_property(component_interfaces_linked "${library}" LIBRARY_COMPONENT_INTERFACES_LINKED)
    foreach(component_interface IN LISTS component_interfaces_linked)
        idf_component_get_property(component_source "${component_interface}" COMPONENT_SOURCE)
        idf_component_get_property(component_target "${component_interface}" COMPONENT_TARGET)
        idf_component_get_property(component_type "${component_interface}" COMPONENT_TYPE)

        if("${component_type}" STREQUAL "CONFIG_ONLY")
            # Configuration only component interface target without a library.
            continue()
        endif()

        if(NOT "${component_source}" STREQUAL "project_components")
            # Add only project components as mutable.
            continue()
        endif()

        idf_library_set_property("${library}" __LDGEN_MUTABLE_LIBS
            "$<TARGET_LINKER_FILE_NAME:${component_target}>" APPEND)
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

        idf_library_get_property(ldgen_depends "${library}" __LDGEN_DEPENDS)
        if(NOT "${dep}" IN_LIST ldgen_depends)
            idf_library_set_property("${library}" __LDGEN_LIBRARIES "$<TARGET_FILE:${dep}>" APPEND)
            idf_library_set_property("${library}" __LDGEN_DEPENDS ${dep} APPEND)
        endif()
    endforeach()

    # Create a sanitized library interface name that can be used as a suffix
    # for files and targets specific to the library.
    string(MAKE_C_IDENTIFIER "_${library}" suffix)

    idf_library_get_property(component_interfaces_linked "${library}" LIBRARY_COMPONENT_INTERFACES_LINKED)
    foreach(component_interface IN LISTS component_interfaces_linked)
        set(scripts)
        idf_component_get_property(component_dir "${component_interface}" COMPONENT_DIR)
        idf_component_get_property(component_build_dir "${component_interface}" COMPONENT_BUILD_DIR)
        idf_component_get_property(preprocessed "${component_interface}" __LINKER_SCRIPTS_PREPROCESSED)

        # Add linker scripts that do not require ldgen processing.
        idf_component_get_property(scripts_static "${component_interface}" LINKER_SCRIPTS)
        __get_absolute_paths(PATHS "${scripts_static}" BASE_DIR "${component_dir}" OUTPUT scripts_static_abs)
        foreach(script IN LISTS scripts_static_abs)
            if(script MATCHES "\\.in$")
                # If the linker script file name ends with the ".in" extension,
                # preprocess it with the C preprocessor.
                get_filename_component(script_name "${script}" NAME_WLE)
                set(script_out "${component_build_dir}/ld/${script_name}")
                string(MAKE_C_IDENTIFIER "gen_${script_name}" script_target)

                if(NOT preprocessed)
                    file(MAKE_DIRECTORY "${component_build_dir}/ld")
                    __preprocess_linker_script("${script}" "${script_out}")
                    # Add a custom target for the preprocessed script.
                    add_custom_target(${script_target} DEPENDS "${script_out}")
                endif()
                # Add dependency for the library interface to ensure the script is
                # generated before linking.
                add_dependencies("${library}" ${script_target})
                list(APPEND scripts "${script_out}")
            else()
                list(APPEND scripts "${script}")
            endif()
        endforeach()

        # Generate linker scripts from templates.
        # LINKER_SCRIPTS_TEMPLATE and LINKER_SCRIPTS_GENERATED are parallel
        # lists. The first holds the template linker script path, and the
        # second holds the generated linker script path.
        idf_component_get_property(template_scripts "${component_interface}" LINKER_SCRIPTS_TEMPLATE)
        idf_component_get_property(generated_scripts "${component_interface}" LINKER_SCRIPTS_GENERATED)

        foreach(template script IN ZIP_LISTS template_scripts generated_scripts)
            if(template MATCHES "\\.in$")
                # If the linker script file name ends with the ".in" extension,
                # preprocess it with the C preprocessor.
                get_filename_component(template_name "${template}" NAME)
                set(template_out "${component_build_dir}/ld/${template_name}")
                if(NOT preprocessed)
                    file(MAKE_DIRECTORY "${component_build_dir}/ld")
                    __preprocess_linker_script("${template}" "${template_out}")
                endif()
                set(template "${template_out}")
            endif()
            set(script "${script}${suffix}")
            __ldgen_process_template(LIBRARY "${library}"
                                     TEMPLATE "${template}"
                                     SUFFIX "${suffix}"
                                     OUTPUT "${script}")
            list(APPEND scripts "${script}")
            # Add a custom target for the generated script and include it as a
            # dependency for the library interface to ensure the script is
            # generated before linking.
            get_filename_component(basename "${script}" NAME)
            string(MAKE_C_IDENTIFIER "${basename}" basename)
            add_custom_target(gen_${basename} DEPENDS "${script}")
            add_dependencies("${library}" gen_${basename})
        endforeach()

        # The static scripts for the given component have already been
        # preprocessed and are identical across all libraries. Therefore, there
        # is no need to preprocess them multiple times. Set the component
        # property to indicate that the linker scripts have already been
        # preprocessed.
        idf_component_set_property("${component_interface}" __LINKER_SCRIPTS_PREPROCESSED YES)

        # Finally, add all preprocessed and ldgen-generated linker scripts to
        # the library interface.
        foreach(script IN LISTS scripts)
            get_filename_component(script_dir "${script}" DIRECTORY)
            get_filename_component(script_name "${script}" NAME)
            # Add linker script directory to the linker search path.
            target_link_directories("${library}" INTERFACE "${script_dir}")
            # Add linker script to link. Regarding the usage of SHELL, see
            # https://cmake.org/cmake/help/latest/command/target_link_options.html#option-de-duplication
            target_link_options("${library}" INTERFACE "SHELL:-T ${script_name}")
            # Add the linker script as a dependency to ensure the executable is
            # re-linked if the script changes.
            set_property(TARGET "${library}" APPEND PROPERTY INTERFACE_LINK_DEPENDS "${script}")
        endforeach()
    endforeach()

    # Validate components linked to this library
    __component_validation_run_checks(LIBRARY "${library}")
endfunction()

#[[
.. cmakev2:function:: idf_build_executable

    .. code-block:: cmake

        idf_build_executable(<executable>
                             [COMPONENTS <component>...]
                             [NAME <name>]
                             [SUFFIX <suffix>])

    *executable[in]*

        Name of the executable target to be created.

    *COMPONENTS[in,opt]*

        List of component names to add to the library.

    *NAME[in,opt]*

        Optional preferred generated binary name. If not provided, the
        ``executable`` name is used.

    *SUFFIX[in,opt]*

        Optional ``executable`` suffix.

    *MAPFILE_TARGET[in,opt]*

        Name of the target for the map file. If provided, the link map file is
        generated for the specified executable, and the ``MAPFILE_TARGET``
        target name is created for it. The ``MAPFILE_PATH`` property with the
        link map file path is added to the ``MAPFILE_TARGET`` target. This can
        be used for other targets that depend on the link map file. The link map file
        is not generated on Darwin host, so the target ``MAPFILE_TARGET`` may not
        be created if link map file is not generated.

    Create a new executable target using the name specified in the
    ``executable`` argument, and link it to the library created with the
    component names provided in the ``COMPONENTS`` option. If the
    ``COMPONENTS`` option is not set, all discovered components are added to
    the library.  Optionally set the executable name and suffix. The executable
    library target name is added to the ``LIBRARY_INTERFACE`` executable
    property.
#]]
function(idf_build_executable executable)
    set(options)
    set(one_value NAME SUFFIX MAPFILE_TARGET)
    set(multi_value COMPONENTS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_NAME)
        set(ARG_NAME "${executable}")
    endif()

    if(NOT DEFINED ARG_SUFFIX)
        set(ARG_SUFFIX ".elf")
    endif()

    idf_build_get_property(build_dir BUILD_DIR)

    set(library "library_${executable}")
    idf_build_library(${library} COMPONENTS "${ARG_COMPONENTS}")

    set(executable_src ${CMAKE_BINARY_DIR}/executable_${executable}.c)
    if(NOT EXISTS "${executable_src}")
        file(TOUCH "${executable_src}")
    endif()
    add_executable(${executable} "${executable_src}")

    set_target_properties(${executable} PROPERTIES OUTPUT_NAME ${ARG_NAME})

    if(ARG_SUFFIX)
        set_target_properties(${executable} PROPERTIES SUFFIX ${ARG_SUFFIX})
    endif()

    target_link_libraries(${executable} PRIVATE ${library})

    idf_build_get_property(linker_type LINKER_TYPE)
    if(ARG_MAPFILE_TARGET AND "${linker_type}" STREQUAL "GNU")
        set(mapfile "${CMAKE_BINARY_DIR}/${ARG_NAME}.map")
        target_link_options(${executable} PRIVATE "LINKER:--Map=${mapfile}")
        add_custom_command(
            OUTPUT "${mapfile}"
            DEPENDS ${executable}
        )
        add_custom_target(${ARG_MAPFILE_TARGET}
            DEPENDS "${mapfile}"
        )
        set_target_properties(${ARG_MAPFILE_TARGET} PROPERTIES MAPFILE_PATH ${mapfile})
    endif()

    set_target_properties(${executable} PROPERTIES LIBRARY_INTERFACE ${library})
endfunction()

#[[
    __get_components_metadata(COMPONENTS <component_interface>...
                              OUTPUT <variable>)

    *COMPONENTS[in]*

        List of component interfaces for which to generate metadata.

    *OUTPUT[out]*

        Output variable to store JSON metadata.

    Generate metadata in JSON format from ``COMPONENTS`` and store it in the
    ``OUTPUT`` variable.
#]]
function(__get_components_metadata)
    set(options)
    set(one_value OUTPUT)
    set(multi_value COMPONENTS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_COMPONENTS)
        idf_die("COMPONENTS option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    set(components_json "")

    foreach(interface IN LISTS ARG_COMPONENTS)
        __idf_component_get_property_unchecked(name ${interface} COMPONENT_NAME)
        __idf_component_get_property_unchecked(target ${interface} COMPONENT_REAL_TARGET)
        __idf_component_get_property_unchecked(alias ${interface} COMPONENT_ALIAS)
        __idf_component_get_property_unchecked(prefix ${interface} __PREFIX)
        __idf_component_get_property_unchecked(dir ${interface} COMPONENT_DIR)
        __idf_component_get_property_unchecked(type ${interface} COMPONENT_TYPE)
        __idf_component_get_property_unchecked(lib ${interface} COMPONENT_LIB)
        __idf_component_get_property_unchecked(reqs ${interface} REQUIRES)
        __idf_component_get_property_unchecked(include_dirs ${interface} INCLUDE_DIRS)
        __idf_component_get_property_unchecked(priv_reqs ${interface} PRIV_REQUIRES)
        __idf_component_get_property_unchecked(managed_reqs ${interface} MANAGED_REQUIRES)
        __idf_component_get_property_unchecked(managed_priv_reqs ${interface} MANAGED_PRIV_REQUIRES)
        __idf_component_get_property_unchecked(component_source ${interface} COMPONENT_SOURCE)
        if("${type}" STREQUAL "LIBRARY")
            set(file "$<TARGET_LINKER_FILE:${lib}>")

            # The idf_component_register function is converting each source file path defined
            # in SRCS into absolute one. But source files can be also added with cmake's
            # target_sources and have relative paths. This is used for example in log
            # component. Let's make sure all source files have absolute path.
            set(sources "")
            get_target_property(srcs ${lib} SOURCES)
            foreach(src IN LISTS srcs)
                get_filename_component(src "${src}" ABSOLUTE BASE_DIR "${dir}")
                list(APPEND sources "${src}")
            endforeach()

        else()
            set(file "")
            set(sources "")
        endif()

        __make_json_list("${reqs}" OUTPUT reqs)
        __make_json_list("${priv_reqs}" OUTPUT priv_reqs)
        __make_json_list("${managed_reqs}" OUTPUT managed_reqs)
        __make_json_list("${managed_priv_reqs}" OUTPUT managed_priv_reqs)
        __make_json_list("${include_dirs}" OUTPUT include_dirs)
        __make_json_list("${sources}" OUTPUT sources)

        string(JOIN "\n" component_json
            "        \"${name}\": {"
            "            \"alias\": \"${alias}\","
            "            \"target\": \"${target}\","
            "            \"prefix\": \"${prefix}\","
            "            \"dir\": \"${dir}\","
            "            \"source\": \"${component_source}\","
            "            \"type\": \"${type}\","
            "            \"lib\": \"${lib}\","
            "            \"reqs\": ${reqs},"
            "            \"priv_reqs\": ${priv_reqs},"
            "            \"managed_reqs\": ${managed_reqs},"
            "            \"managed_priv_reqs\": ${managed_priv_reqs},"
            "            \"file\": \"${file}\","
            "            \"sources\": ${sources},"
            "            \"include_dirs\": ${include_dirs}"
            "        }"
        )
        string(CONFIGURE "${component_json}" component_json)
        if(NOT "${components_json}" STREQUAL "")
            string(APPEND components_json ",\n")
        endif()
        string(APPEND components_json "${component_json}")
    endforeach()
    string(PREPEND components_json "{\n")
    string(APPEND components_json "\n    }")
    set(${ARG_OUTPUT} "${components_json}" PARENT_SCOPE)
endfunction()

#[[
.. cmakev2:function:: idf_build_generate_metadata

    .. code-block:: cmake

        idf_build_generate_metadata(<binary>
                                    [OUTPUT_FILE <file>])

    *binary[in]*

        Binary target for which to generate a metadata file.

    *OUTPUT_FILE[in,opt]*

        Optional output file path for storing the metadata. If not provided,
        the default path ``<build>/project_description.json`` is used.

    Generate metadata for the specified ``binary`` and store it in the
    specified ``OUTPUT_FILE``. If no ``OUTPUT_FILE`` is provided, the default
    location ``<build>/project_description.json`` will be used.
#]]
function(idf_build_generate_metadata binary)
    set(options)
    set(one_value OUTPUT_FILE)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    # The EXECUTABLE_TARGET property is set by the idf_build_binary or
    # the idf_sign_binary function.
    get_target_property(executable "${binary}" EXECUTABLE_TARGET)
    if(NOT executable)
        idf_die("Binary target '${binary}' is missing 'EXECUTABLE_TARGET' property.")
    endif()
    __get_executable_library_or_die(TARGET "${executable}" OUTPUT library)

    idf_build_get_property(PROJECT_NAME PROJECT_NAME)
    idf_build_get_property(PROJECT_VER PROJECT_VER)
    idf_build_get_property(PROJECT_PATH PROJECT_DIR)
    idf_build_get_property(IDF_PATH IDF_PATH)
    idf_build_get_property(BUILD_DIR BUILD_DIR)
    idf_build_get_property(SDKCONFIG SDKCONFIG)
    idf_build_get_property(SDKCONFIG_DEFAULTS SDKCONFIG_DEFAULTS)
    set(PROJECT_EXECUTABLE "$<TARGET_FILE_NAME:${executable}>")
    # The BINARY_PATH property is set by the idf_build_binary or
    # the idf_sign_binary function.
    get_target_property(binary_path ${binary} BINARY_PATH)
    if(NOT binary_path)
        idf_die("Binary target '${binary}' is missing 'BINARY_PATH' property.")
    endif()
    get_filename_component(PROJECT_BIN "${binary_path}" NAME)
    if(NOT PROJECT_BIN)
        set(PROJECT_BIN "")
    endif()
    if(CONFIG_APP_BUILD_TYPE_RAM)
        set(PROJECT_BUILD_TYPE ram_app)
    else()
        set(PROJECT_BUILD_TYPE flash_app)
    endif()
    idf_build_get_property(IDF_VER IDF_VER)

    idf_build_get_property(common_component_reqs __COMPONENT_REQUIRES_COMMON)
    list(SORT common_component_reqs)
    __make_json_list("${common_component_reqs}" OUTPUT common_component_reqs_json)

    idf_library_get_property(build_components "${library}" LIBRARY_COMPONENTS_LINKED)
    idf_library_get_property(build_component_interfaces "${library}" LIBRARY_COMPONENT_INTERFACES_LINKED)
    list(SORT build_components)
    list(SORT build_component_interfaces)
    __make_json_list("${build_components}" OUTPUT build_components_json)

    set(build_component_paths)
    set(COMPONENT_KCONFIGS)
    set(COMPONENT_KCONFIGS_PROJBUILD)
    foreach(component_interface IN LISTS build_component_interfaces)
        __idf_component_get_property_unchecked(component_dir "${component_interface}" COMPONENT_DIR)
        __idf_component_get_property_unchecked(component_kconfig "${component_interface}" __KCONFIG)
        __idf_component_get_property_unchecked(component_kconfig_projbuild "${component_interface}" __KCONFIG_PROJBUILD)
        list(APPEND build_component_paths "${component_dir}")
        if(component_kconfig)
            list(APPEND COMPONENT_KCONFIGS "${component_kconfig}")
        endif()
        if(component_kconfig_projbuild)
            list(APPEND COMPONENT_KCONFIGS_PROJBUILD "${component_kconfig_projbuild}")
        endif()
    endforeach()
    __make_json_list("${build_component_paths}" OUTPUT build_component_paths_json)

    __get_components_metadata(COMPONENTS "${build_component_interfaces}" OUTPUT build_component_info_json)

    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
    __get_components_metadata(COMPONENTS "${component_interfaces}" OUTPUT all_component_info_json)

    __generate_gdbinit()
    idf_build_get_property(gdbinit_files_prefix_map GDBINIT_FILES_PREFIX_MAP)
    idf_build_get_property(gdbinit_files_symbols GDBINIT_FILES_SYMBOLS)
    idf_build_get_property(gdbinit_files_py_extensions GDBINIT_FILES_PY_EXTENSIONS)
    idf_build_get_property(gdbinit_files_connect GDBINIT_FILES_CONNECT)
    __get_openocd_options(debug_arguments_openocd)

    if(NOT DEFINED ARG_OUTPUT_FILE)
        set(ARG_OUTPUT_FILE "${BUILD_DIR}/project_description.json")
    endif()

    get_filename_component(ARG_OUTPUT_FILE "${ARG_OUTPUT_FILE}" ABSOLUTE BASE_DIR "${BUILD_DIR}")

    configure_file("${IDF_PATH}/tools/cmake/project_description.json.in" "${ARG_OUTPUT_FILE}.templ")
    file(READ "${ARG_OUTPUT_FILE}.templ" project_description_json_templ)
    file(REMOVE "${ARG_OUTPUT_FILE}.templ")
    file(GENERATE OUTPUT "${ARG_OUTPUT_FILE}"
         CONTENT "${project_description_json_templ}")
endfunction()

#[[
.. cmakev2:function:: idf_build_binary

    .. code-block:: cmake

        idf_build_binary(<executable>
                         TARGET <target>
                         OUTPUT_FILE <file>)

    *executable[in]*

        Executable target for which to generate a binary image file.

    *TARGET[in]*

        The name of the target that will be created for the generated binary
        image.

    *OUTPUT_FILE[in]*

        Output file path for storing the binary image file.

    Create a binary image for the specified ``executable`` target and save it
    in the file specified with the ``OUTPUT_FILE`` option. A custom target
    named ``TARGET`` will be created for the generated binary image. The path
    of the generated binary image will be stored in the ``BINARY_PATH``
    property and the executable target in the ``EXECUTABLE_TARGET`` property of
    the ``TARGET``.
#]]
function(idf_build_binary executable)
    set(options)
    set(one_value OUTPUT_FILE TARGET)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT TARGET idf::esptool_py)
        idf_die("The 'esptool_py' component is not available")
    endif()

    if(NOT CONFIG_APP_BUILD_GENERATE_BINARIES)
        idf_die("Binary file generation is not enabled with 'CONFIG_APP_BUILD_GENERATE_BINARIES'")
    endif()

    if(NOT TARGET "${executable}")
        idf_die("The executable '${executable}' is not a cmake target")
    endif()

    get_target_property(type "${executable}" TYPE)
    if(NOT "${type}" STREQUAL "EXECUTABLE")
        idf_die("The executable target '${executable}' is not of the EXECUTABLE type")
    endif()

    idf_build_get_property(build_dir BUILD_DIR)

    get_target_property(executable_name ${executable} OUTPUT_NAME)
    if(NOT executable_name)
        set(executable_name "${executable}")
    endif()

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT_FILE)
        idf_die("OUTPUT_FILE option is required")
    endif()

    idf_component_get_property(esptool_py_cmd esptool_py ESPTOOLPY_CMD)
    idf_component_get_property(esptool_elf2image_args esptool_py ESPTOOL_PY_ELF2IMAGE_ARGS)

    get_filename_component(binary_name "${ARG_OUTPUT_FILE}" NAME)

    # Create a custom command and target to generate a binary from an ELF file.
    add_custom_command(OUTPUT "${ARG_OUTPUT_FILE}"
        COMMAND ${esptool_py_cmd} elf2image ${esptool_elf2image_args}
        -o "${ARG_OUTPUT_FILE}" "$<TARGET_FILE:${executable}>"
        COMMAND ${CMAKE_COMMAND} -E echo "Generated ${ARG_OUTPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} -E md5sum "${ARG_OUTPUT_FILE}" > "${ARG_OUTPUT_FILE}.md5sum"
        DEPENDS ${executable}
        VERBATIM
        WORKING_DIRECTORY ${build_dir}
        COMMENT "Generating binary image '${binary_name}' from executable '${executable_name}'"
    )

    # Create a custom target to generate the binary file
    add_custom_target(${ARG_TARGET} DEPENDS "${ARG_OUTPUT_FILE}")

    # Store the path of the binary file in the BINARY_PATH property of the
    # custom binary target, which is used by the idf_flash_binary.
    set_target_properties(${ARG_TARGET} PROPERTIES BINARY_PATH ${ARG_OUTPUT_FILE})

    # Store executable target name in the EXECUTABLE_TARGET property. This is used
    # by the idf_build_generate_metadata function.
    set_target_properties(${ARG_TARGET} PROPERTIES EXECUTABLE_TARGET ${executable})
endfunction()

#[[
.. cmakev2:function:: idf_sign_binary

    .. code-block:: cmake

        idf_sign_binary(<binary>
                        TARGET <target>
                        OUTPUT_FILE <file>
                        [KEYFILE <file>])

    *binary[in]*

        Binary image target for which to generate a signed binary image file.
        The ``binary`` target is created by the :cmakev2:ref:`idf_build_binary`
        function.

    *TARGET[in]*

        The name of the target that will be created for the signed binary
        image.

    *OUTPUT_FILE[in]*

        Output file path for storing the signed binary image file.

    *KEYFILE[in,opt]*

        Optional path to the key file that should be used for signing. If not
        provided, the key file specified by the
        ``CONFIG_SECURE_BOOT_SIGNING_KEY`` configuration option will be used.

    Sign binary image specified by ``binary`` target with ``KEYFILE`` and save
    it in the file specified with the  `OUTPUT_FILE` option. A custom target
    named ``TARGET`` will be created for the signed binary image.  The path of
    the signed binary image will be stored in the ``BINARY_PATH`` property and
    the executable target in the ``EXECUTABLE_TARGET`` property of the
    ``TARGET``.
#]]
function(idf_sign_binary binary)
    set(options)
    set(one_value OUTPUT_FILE TARGET KEYFILE)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT TARGET idf::esptool_py)
        idf_die("The 'esptool_py' component is not available")
    endif()

    if(NOT CONFIG_APP_BUILD_GENERATE_BINARIES)
        idf_die("Binary file generation is not enabled with 'CONFIG_APP_BUILD_GENERATE_BINARIES'")
    endif()

    if(NOT CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        idf_die("Binary file signing is not enabled with 'CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES'")
    endif()

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT_FILE)
        idf_die("OUTPUT_FILE option is required")
    endif()

    if(ARG_KEYFILE)
        set(keyfle "${ARG_KEYFILE}")
    else()
        idf_build_get_property(project_dir PROJECT_DIR)
        get_filename_component(keyfile "${CONFIG_SECURE_BOOT_SIGNING_KEY}" ABSOLUTE BASE_DIR "${project_dir}")
    endif()

    if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
        set(secure_boot_version "1")
    elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
        set(secure_boot_version "2")
    endif()

    idf_component_get_property(espsecure_py_cmd esptool_py ESPSECUREPY_CMD)
    get_target_property(binary_path ${binary} BINARY_PATH)
    if(NOT binary_path)
        idf_die("Binary target '${binary}' is missing 'BINARY_PATH' property.")
    endif()
    get_filename_component(binary_name "${binary_path}" NAME)
    get_filename_component(signed_binary_name "${ARG_OUTPUT_FILE}" NAME)
    get_filename_component(key_name "${keyfile}" NAME)

    add_custom_command(OUTPUT "${ARG_OUTPUT_FILE}"
        COMMAND ${espsecure_py_cmd} sign-data
        --version ${secure_boot_version} --keyfile "${keyfile}"
        -o "${ARG_OUTPUT_FILE}" "${binary_path}"
        COMMAND ${CMAKE_COMMAND} -E md5sum "${ARG_OUTPUT_FILE}" > "${ARG_OUTPUT_FILE}.md5sum"
        DEPENDS "${binary}"
        VERBATIM
        COMMENT "Signing '${binary_name}' with key '${key_name}' into '${signed_binary_name}'"
    )
    add_custom_target(${ARG_TARGET} DEPENDS "${ARG_OUTPUT_FILE}")

    # Store the path of the binary file in the BINARY_PATH property of the
    # custom signed binary target, which is used by the idf_flash_binary.
    set_target_properties(${ARG_TARGET} PROPERTIES BINARY_PATH ${ARG_OUTPUT_FILE})

    # Store executable target name in the EXECUTABLE_TARGET property. This is used
    # by the idf_build_generate_metadata function.
    get_target_property(executable "${binary}" EXECUTABLE_TARGET)
    if(NOT executable)
        idf_die("Binary target '${binary}' is missing 'EXECUTABLE_TARGET' property.")
    endif()
    set_target_properties(${ARG_TARGET} PROPERTIES EXECUTABLE_TARGET ${executable})
endfunction()

#[[
.. cmakev2:function:: idf_flash_binary

    .. code-block:: cmake

        idf_flash_binary(<binary>
                         TARGET <target>
                         [NAME <name>]
                         [FLASH])

    *binary[in]*

        Binary image target for which to create flash target.  The ``binary``
        target is created by the :cmakev2:ref:`idf_build_binary` function or
        :cmakev2:ref:`idf_sign_binary`.

    *TARGET[in]*

        The name of the flash target that will be created for the binary image.

    *NAME[in,opt]*

        An optional name to be used as a prefix for the file containing
        arguments for esptool, and as a logical name for the binary in
        ``flasher_args.json``. If not specified, the name of the ``TARGET`` is
        used.

    *FLASH[in,opt]*

        If specified, the binary will also be added to the global ``flash``
        target.

    Create a new flash target for ``binary`` using the name specified in the
    ``TARGET`` option. The file path of the binary image should be stored in
    the ``BINARY_PATH`` property of the ``binary`` target.
#]]
function(idf_flash_binary binary)
    set(options FLASH)
    set(one_value TARGET NAME)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT TARGET idf::esptool_py)
        idf_die("The 'esptool_py' component is not available")
    endif()

    if(NOT CONFIG_APP_BUILD_GENERATE_BINARIES)
        idf_die("Binary file generation is not enabled with 'CONFIG_APP_BUILD_GENERATE_BINARIES'")
    endif()

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(NOT DEFINED ARG_NAME)
        set(ARG_NAME "${ARG_TARGET}")
    endif()

    idf_component_get_property(main_args esptool_py FLASH_ARGS)
    idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)

    partition_table_get_partition_info(offset "--partition-boot-default" "offset")
    get_target_property(binary_path ${binary} BINARY_PATH)
    if(NOT binary_path)
        idf_die("Binary target '${binary}' is missing 'BINARY_PATH' property.")
    endif()

    idf_build_get_property(build_dir BUILD_DIR)
    get_filename_component(binary_path "${binary_path}" ABSOLUTE BASE_DIR "${build_dir}")

    esptool_py_flash_target("${ARG_TARGET}" "${main_args}" "${sub_args}"
                            FILENAME_PREFIX "${ARG_NAME}")
    esptool_py_flash_target_image("${ARG_TARGET}" "${ARG_NAME}" "${offset}" "${binary_path}")
    add_dependencies("${ARG_TARGET}" "${binary}")

    if(ARG_FLASH)
        esptool_py_flash_target_image(flash ${ARG_NAME} "${offset}" "${binary_path}")
        add_dependencies(flash "${binary}")
    endif()
endfunction()

#[[
.. cmakev2:function:: idf_check_binary_size

    .. code-block:: cmake

        idf_check_binary_size(<binary>)

    *binary[in]*

        Binary image target to which to add a partition size check.  The
        ``binary`` target is created by the :cmakev2:ref:`idf_build_binary` or
        :cmakev2:ref:`idf_sign_binary` function.

    Ensure that the generated binary image fits within the smallest available
    application partition. The file path of the binary image should be stored
    in the ``BINARY_PATH`` property of the ``binary`` target.
#]]
function(idf_check_binary_size binary)
    if(NOT CONFIG_APP_BUILD_TYPE_APP_2NDBOOT)
        return()
    endif()

    get_target_property(binary_path ${binary} BINARY_PATH)
    if(NOT binary_path)
        idf_die("Binary target '${binary}' is missing 'BINARY_PATH' property.")
    endif()

    partition_table_add_check_size_target("${binary}_check_size"
        DEPENDS "${binary_path}"
        BINARY_PATH "${binary_path}"
        PARTITION_TYPE app)
    add_dependencies("${binary}" "${binary}_check_size")
endfunction()

#[[
.. cmakev2:function:: idf_check_binary_signed

    .. code-block:: cmake

        idf_check_binary_signed(<binary>)

    *binary[in]*

        Binary image target to which to add a partition size check.  The
        ``binary`` target is created by the ``idf_build_binary``.

    If the binary is not signed and signed applications are required with
    CONFIG_SECURE_SIGNED_APPS, print a note indicating that the binary needs to
    be signed manually. This situation can occur if the binary images are not
    signed during the build because CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES is
    not set.
#]]
function(idf_check_binary_signed binary)
    if(NOT CONFIG_SECURE_SIGNED_APPS)
        # Signed binaries are not required.
        return()
    endif()

    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        # Binary should be already signed with idf_sign_binary.
        return()
    endif()

    get_target_property(binary_path ${binary} BINARY_PATH)
    if(NOT binary_path)
        idf_die("Binary target '${binary}' is missing 'BINARY_PATH' property.")
    endif()

    if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
        set(secure_boot_version "1")
    elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
        set(secure_boot_version "2")
    endif()

    idf_component_get_property(espsecure_py_cmd esptool_py ESPSECUREPY_CMD)
    string(REPLACE ";" " " espsecure_py_cmd "${espsecure_py_cmd}")
    get_filename_component(binary_name "${binary_path}" NAME)

    add_custom_command(TARGET "${binary}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "Binary '${binary_name}' not signed. Sign it before flashing with:"
        COMMAND ${CMAKE_COMMAND} -E echo
        "${espsecure_py_cmd} sign-data --keyfile KEYFILE --version ${secure_boot_version}"
        "${binary_path}"
        VERBATIM)
endfunction()

#[[
.. cmakev2:function:: idf_build_generate_depgraph

    .. code-block:: cmake

        idf_build_generate_depgraph(<executable>
                                    [OUTPUT_FILE <file>])

    *executable[in]*

        Executable target for which to generate a dependency graph.

    *OUTPUT_FILE[in,opt]*

        Optional output file path for storing the dependency graph. If not
        provided, the default path ``<build>/component_deps.dot`` is used.

    Generate dependency graph for the specified ``executable`` and store it in
    the specified ``OUTPUT_FILE``. If no ``OUTPUT_FILE`` is provided, the
    default location ``<build>/component_deps.dot`` will be used.
#]]
function(idf_build_generate_depgraph executable)
    set(options)
    set(one_value OUTPUT_FILE)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    idf_build_get_property(depgraph_enabled __BUILD_COMPONENT_DEPGRAPH_ENABLED)
    if(NOT depgraph_enabled)
        return()
    endif()

    __get_executable_library_or_die(TARGET "${executable}" OUTPUT library)

    idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH "")
    idf_build_get_property(common_reqs __COMPONENT_REQUIRES_COMMON)

    idf_library_get_property(build_components "${library}" LIBRARY_COMPONENTS_LINKED)

    foreach(component_name IN LISTS build_components)
        idf_component_get_property(reqs "${component_name}" REQUIRES)
        idf_component_get_property(component_format "${component_name}" COMPONENT_FORMAT)
        if("${component_format}" STREQUAL "CMAKEV1")
            # For cmakev1 components, also include commonly required
            # components.
            list(APPEND reqs ${common_reqs})
        endif()

        foreach(req IN LISTS reqs)
            depgraph_add_edge(${component_name} ${req} REQUIRES)
        endforeach()

        idf_component_get_property(priv_reqs "${component_name}" PRIV_REQUIRES)
        foreach(priv_req IN LISTS priv_reqs)
            depgraph_add_edge(${component_name} ${priv_req} PRIV_REQUIRES)
        endforeach()
    endforeach()

    if(NOT DEFINED ARG_OUTPUT_FILE)
        idf_build_get_property(build_dir BUILD_DIR)
        set(ARG_OUTPUT_FILE "${build_dir}/component_deps.dot")
    endif()

    depgraph_generate("${ARG_OUTPUT_FILE}")
endfunction()
