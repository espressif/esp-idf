# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

#[[
    check_expected_tool_version()

    Function to verify if the tool is using the expected version and issue a
    warning if it is not.
#]]
function(check_expected_tool_version tool_name tool_path)
    set(tool_version_warning "Check Getting Started documentation or proceed at own risk.\n")
    set(tool_version_error "Check Getting Started documentation if the error continues.\n"
        "You can override this error and proceed with build by defining the IDF_MAINTAINER environment variable.\n")
    set(fixing_hint "Please try to run 'idf.py fullclean' to solve it.\n")

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)

    set(ENV{IDF_TOOLS_VERSION_HELPER} "1")
    # Use idf_tools.py to check if tool version is supported
    execute_process(
        COMMAND ${python} "${idf_path}/tools/idf_tools.py"
        "check-tool-supported" "--tool-name" "${tool_name}"
        "--exec-path" "${tool_path}"
        OUTPUT_VARIABLE is_version_supported
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

    if(is_version_supported STREQUAL "False")
        # Version is not supported. Need to get supported versions list to print them to user
        execute_process(
            COMMAND ${python} "${idf_path}/tools/idf_tools.py"
            "get-tool-supported-versions" "--tool-name" "${tool_name}"
            OUTPUT_VARIABLE tool_supported_versions
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        # IDF maintainers can build projects with not supported versions with just a warning
        if($ENV{IDF_MAINTAINER})
            set(message_mode "WARNING")
        else()
            set(message_mode "FATAL_ERROR")
        endif()

        idf_msg(${message_mode} "\n"
                 "Tool doesn't match supported version from list "
                "${tool_supported_versions}: ${tool_path}\n"
                 ${fixing_hint})
    elseif(NOT is_version_supported STREQUAL "True")
        idf_warn("Can not get version for tool: ${tool_path}\n" ${tool_version_warning})
    endif()
    unset(ENV{IDF_TOOLS_VERSION_HELPER})
endfunction()

#[[
    __get_component_sources([SRCS <src>...]
                            [SRC_DIRS <dir>...]
                            [EXCLUDE_SRCS <exclude>...]
                            OUTPUT <var>)

    *SRCS[in,opt]*

        Optional list of sources.

    *SRC_DIRS[in,opt]*

        Option list of source directories.

    *EXCLUDE_SRCS[in,opt]*

        Optional list of sources to exclude.

    *OUTPUT[out]*

        Output variable to store the list of component sources.

    This helper function gathers component sources from the arguments specified
    in either ``SRCS`` or ``SRC_DIRS``. If both are provided, the ``SRC_DIRS``
    option is disregarded. The sources are collected as absolute paths relative
    to the component's directory. If ``EXCLUDE_SRCS`` is specified, the source
    files listed in it are excluded. The final list of component sources is
    returned in the ``OUTPUT`` variable.

    The ``COMPONENT_NAME`` and ``COMPONENT_DIR`` variables are provided by the
    build system when ``idf_component_register`` is called.
#]]
function(__get_component_sources)
    set(options)
    set(one_value OUTPUT)
    set(multi_value SRCS SRC_DIRS EXCLUDE_SRCS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    if(DEFINED ARG_SRCS AND DEFINED ARG_SRC_DIRS)
        idf_warn("SRCS and SRC_DIRS are both specified for '${COMPONENT_NAME}' "
                 "in '${COMPONENT_DIR}'. Ignoring SRC_DIRS.")
        unset(ARG_SRC_DIRS)
    endif()

    set(sources)

    if(ARG_SRCS)
        __get_absolute_paths(PATHS "${ARG_SRCS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT sources)
    elseif(ARG_SRC_DIRS)
        __get_absolute_paths(PATHS "${ARG_SRC_DIRS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT dirs)
        foreach(dir IN LISTS dirs)
            if(NOT IS_DIRECTORY "${dir}")
                idf_die("SRC_DIRS entry '${dir}' does not exist for component "
                        "'${COMPONENT_NAME}' in '${COMPONENT_DIR}'.")
            endif()

            file(GLOB dir_sources "${dir}/*.c" "${dir}/*.cpp" "${dir}/*.S")
            list(SORT dir_sources)

            if(NOT dir_sources)
                idf_warn("No source files found for SRC_DIRS entry '${dir}' for "
                         "'${COMPONENT_NAME}' in '${COMPONENT_DIR}'.")
                continue()
            endif()

            list(APPEND sources "${dir_sources}")
        endforeach()
    endif()

    if(ARG_EXCLUDE_SRCS)
        __get_absolute_paths(PATHS "${ARG_EXCLUDE_SRCS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT exclude_srcs)
        foreach(src IN LISTS exclude_srcs)
            list(REMOVE_ITEM sources "${src}")
        endforeach()
    endif()

    list(REMOVE_DUPLICATES sources)
    set(${ARG_OUTPUT} "${sources}" PARENT_SCOPE)
endfunction()

#[[
.. cmakev2:function:: target_linker_script

    .. code-block:: cmake

        target_linker_script(<target> <deptype> <scriptfile>...
                             [PROCESS <output>])

    *target[in]*

        The component target to which linker script files should be added.

    *deptype[in]*

        This option is obsolete and maintained solely for backward
        compatibility.

    *scriptfile[in]*

        Specifies the linker script file ``scriptfile`` to be added to the
        link.  Multiple files can be specified.

    *PROCESS[in,opt]*

        Specifies the ``output`` linker script, which is generated from the
        ``linkerscript`` template. The ``linkerscript`` is processed with ldgen
        to produce the ``output``.

    This function adds one or more linker scripts to the specified component
    target, incorporating the linker script into the linking process.

    If the ``PROCESS`` option is specified, the last ``scriptfile`` listed is
    processed using the ldgen command, and the generated ``output`` file is
    used as the linker script during the linking process. This implies that
    with the ``PROCESS`` option, it is logical to provide only a single
    ``scriptfile`` as a template.
#]]
function(target_linker_script target deptype scriptfiles)
    # The linker script files, templates, and their output filenames are stored
    # only as component properties. The script files are generated and added to
    # the library link interface in the idf_build_library function.
    set(options)
    set(one_value PROCESS)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})
    foreach(scriptfile ${scriptfiles})
        get_filename_component(scriptfile "${scriptfile}" ABSOLUTE)
        idf_msg("Adding linker script ${scriptfile}")
        if(ARG_PROCESS)
            get_filename_component(output "${ARG_PROCESS}" ABSOLUTE)
            idf_component_set_property("${target}" LINKER_SCRIPTS_TEMPLATE "${scriptfile}" APPEND)
            idf_component_set_property("${target}" LINKER_SCRIPTS_GENERATED "${output}" APPEND)
        else()
            idf_component_set_property("${target}" LINKER_SCRIPTS ${scriptfile} APPEND)
        endif()
    endforeach()
endfunction()

#[[
.. cmakev2:function:: idf_component_optional_requires

    .. code-block:: cmake

        idf_component_optional_requires(<type> <component>...)

    *type[in]*

        Type of dependency: PRIVATE, PUBLIC or INTERFACE

    *component[in]*

        The component name that should be added as a dependency to the
        evaluated component. It may be provided multiple times.

    Add a dependency on a specific component only if it is included in the
    build.
#]]
function(idf_component_optional_requires req_type)
    set(optional_reqs ${ARGN})
    foreach(req ${optional_reqs})
        __get_component_interface(COMPONENT "${req}" OUTPUT req_interface)
        if("${req_interface}" STREQUAL "NOTFOUND")
            continue()
        endif()
        idf_component_get_property(req_alias "${req}" COMPONENT_ALIAS)
        # The component alias is created only after the component is included,
        # meaning the add_subdirectory command for it has been called. This can
        # be used to detect if a component has already been added to the build.
        target_link_libraries(${COMPONENT_TARGET} ${req_type} "$<$<TARGET_EXISTS:${req_alias}>:${req_interface}>")
    endforeach()
endfunction()

#[[
    __init_common_components()

    Identify the commonly required components based on the target and
    architecture, and store them in the __COMPONENT_REQUIRES_COMMON build
    property. Their interfaces are stored in the __COMMON_COMPONENT_INTERFACES
    build property. The commonly required component interfaces are
    automatically linked to each cmakev1 component added through the
    idf_component_register function.

    This function is called from the idf_component_register function and is
    evaluated only once per project, as ensured by the
    __COMMON_COMPONENTS_INITIALIZED build property. The cmakev2 components are
    expected to properly specify all their dependencies, rather than relying on
    common components to be automatically linked to them. Therefore, the common
    components are relevant only within the context of cmakev1 components.
#]]
function(__init_common_components)
    idf_build_get_property(common_components_initialized __COMMON_COMPONENTS_INITIALIZED)
    if(common_components_initialized)
        return()
    endif()

    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(idf_target_arch IDF_TARGET_ARCH)

    # Define common components that are included as dependencies for each
    # component.
    if("${idf_target}" STREQUAL "linux")
        set(requires_common freertos esp_hw_support heap log soc hal esp_rom esp_common esp_system linux
                            esp_stdio)
    else()
        set(requires_common cxx esp_libc freertos esp_hw_support heap log soc hal esp_rom esp_common
                            esp_system esp_stdio ${idf_target_arch})
    endif()
    idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${requires_common}")

    # Set the common component interfaces first, before including them, so the
    # idf_component_include function can see the complete list of common
    # component interfaces.
    set(common_component_interfaces "")
    foreach(component_name IN LISTS requires_common)
        idf_component_get_property(component_interface "${component_name}" COMPONENT_INTERFACE)
        list(APPEND common_component_interfaces "${component_interface}")
    endforeach()
    idf_build_set_property(__COMMON_COMPONENT_INTERFACES "${common_component_interfaces}")

    foreach(component_name IN LISTS requires_common)
        idf_component_include("${component_name}")
    endforeach()

    idf_build_set_property(__COMMON_COMPONENTS_INITIALIZED YES)
endfunction()

#[[api
.. cmakev2:function:: idf_component_register

    .. code-block:: cmake

        idf_component_register([SRCS <file>..]
                               [SRC_DIRS <dir>...]
                               [EXCLUDE_SRCS <file>...]
                               [INCLUDE_DIRS <dir>...]
                               [PRIV_INCLUDE_DIRS <dir>...]
                               [LDFRAGMENTS <file>...]
                               [REQUIRES <component>...]
                               [PRIV_REQUIRES <component>...]
                               [REQUIRED_IDF_TARGETS <target>...]
                               [EMBED_FILES <file>...]
                               [EMBED_TXTFILES <file>...]
                               [WHOLE_ARCHIVE])

    *SRCS[in,opt]*

        List of source files for the component.

    *SRC_DIRS[in,opt]*

        List of source directories to search for source files (.c, .cpp, .S);
        ignored when SRCS is specified.

    *EXCLUDE_SRCS[in,opt]*

        List of source files to exclude from the designated source directories.

    *INCLUDE_DIRS[in,opt]*

        List of public include directories for the created component library.

    *PRIV_INCLUDE_DIRS[in,opt]*

        List of private include directories for the newly created component
        library.

    *LDFRAGMENTS[in,opt]*

        List of linker script fragments for the component.

    *REQUIRES[in,opt]*

        List of publicly required components based on usage requirements.

    *PRIV_REQUIRES[in,opt]*

        List of privately required components based on usage requirements.

    *REQUIRED_IDF_TARGETS[in,opt]*

        List of IDF build targets supported exclusively by the component.

    *EMBED_FILES[in,opt]*

        List of binary files to embed with the component.

    *EMBED_TXTFILES[in,opt]*

        List of text files to embed with the component.

    *WHOLE_ARCHIVE[in,opt]*

        Link the component as --whole-archive.

    Register a new component with the build system using the provided options.
    This function also automatically links all commonly required and managed
    components to the component's target.
#]]
function(idf_component_register)
    set(options WHOLE_ARCHIVE)
    set(one_value KCONFIG KCONFIG_PROJBUILD)
    set(multi_value SRCS SRC_DIRS EXCLUDE_SRCS
                    INCLUDE_DIRS PRIV_INCLUDE_DIRS LDFRAGMENTS REQUIRES
                    PRIV_REQUIRES REQUIRED_IDF_TARGETS EMBED_FILES EMBED_TXTFILES)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    # Initialize and include commonly required components.
    __init_common_components()

    if(ARG_REQUIRED_IDF_TARGETS)
        idf_build_get_property(idf_target IDF_TARGET)
        if(NOT idf_target IN_LIST ARG_REQUIRED_IDF_TARGETS)
            idf_die("Component ${COMPONENT_NAME} only supports targets: ${ARG_REQUIRED_IDF_TARGETS}")
        endif()
    endif()

    if(ARG_KCONFIG)
        get_filename_component(filename "${ARG_KCONFIG}" NAME)
        string(TOLOWER "${filename}" filename)
        if(NOT "${filename}" STREQUAL "kconfig")
            idf_die("The component '${COMPONENT_NAME}' in the '${COMPONENT_DIR}' directory "
                    "is specifying '${ARG_KCONFIG}' as the 'KCONFIG' option, which is not "
                    "the default 'Kconfig' filename.")
        endif()
    endif()

    if(ARG_KCONFIG_PROJBUILD)
        get_filename_component(filename "${ARG_KCONFIG_PROJBUILD}" NAME)
        string(TOLOWER "${filename}" filename)
        if(NOT "${filename}" STREQUAL "kconfig.projbuild")
            idf_die("The component '${COMPONENT_NAME}' in the '${COMPONENT_DIR}' directory "
                    "is specifying '${ARG_KCONFIG_PROJBUILD}' as the 'KCONFIG_PROJBUILD' option, "
                    "which is not the default 'Kconfig.projbuild' filename.")
        endif()
    endif()

    __get_component_sources(SRCS "${ARG_SRCS}"
                            SRC_DIRS "${ARG_SRC_DIRS}"
                            EXCLUDE_SRCS "${ARG_EXCLUDE_SRCS}"
                            OUTPUT sources)

    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    include_directories("${include_directories}")

    idf_build_get_property(compile_definitions COMPILE_DEFINITIONS GENERATOR_EXPRESSION)
    add_compile_definitions("${compile_definitions}")

    __get_compile_options(OUTPUT compile_options)
    add_compile_options("${compile_options}")

    idf_build_get_property(common_component_interfaces __COMMON_COMPONENT_INTERFACES)
    idf_component_get_property(component_interface "${COMPONENT_NAME}" COMPONENT_INTERFACE)
    list(REMOVE_ITEM common_component_interfaces ${component_interface})
    link_libraries(${common_component_interfaces})

    __get_absolute_paths(PATHS "${ARG_INCLUDE_DIRS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT include_dirs)
    __get_absolute_paths(PATHS "${ARG_PRIV_INCLUDE_DIRS}" BASE_DIR "${COMPONENT_DIR}" OUTPUT priv_include_dirs)
    foreach(dir IN LISTS include_dirs priv_include_dirs)
        if(NOT IS_DIRECTORY ${dir})
            idf_die("The component '${COMPONENT_NAME}' in the '${COMPONENT_DIR}' directory "
                    "is specifying '${dir}' as its include directory, but the directory does "
                    "not exists.")
        endif()
    endforeach()

    if(sources OR ARG_EMBED_FILES OR ARG_EMBED_TXTFILES)
        add_library("${COMPONENT_TARGET}" STATIC "${sources}")

        foreach(include_dir IN LISTS include_dirs)
            target_include_directories("${COMPONENT_TARGET}" PUBLIC "${include_dir}")
        endforeach()

        foreach(include_dir IN LISTS priv_include_dirs)
            target_include_directories("${COMPONENT_TARGET}" PRIVATE "${include_dir}")
        endforeach()

        set_target_properties(${COMPONENT_TARGET} PROPERTIES OUTPUT_NAME ${COMPONENT_NAME} LINKER_LANGUAGE C)

        set(component_type LIBRARY)

    else()
        add_library("${COMPONENT_TARGET}" INTERFACE)

        foreach(include_dir IN LISTS include_dirs)
            target_include_directories("${COMPONENT_TARGET}" INTERFACE "${include_dir}")
        endforeach()

        set(component_type CONFIG_ONLY)
    endif()

    foreach(req IN LISTS ARG_REQUIRES)
        idf_component_include("${req}")
        idf_component_get_property(req_interface "${req}" COMPONENT_INTERFACE)
        if(${component_type} STREQUAL LIBRARY)
            target_link_libraries("${COMPONENT_TARGET}" PUBLIC "${req_interface}")
        else()
            target_link_libraries("${COMPONENT_TARGET}" INTERFACE "${req_interface}")
        endif()
    endforeach()

    foreach(req IN LISTS ARG_PRIV_REQUIRES)
        idf_component_include("${req}")
        idf_component_get_property(req_interface "${req}" COMPONENT_INTERFACE)
        if(${component_type} STREQUAL CONFIG_ONLY)
            continue()
        endif()
        target_link_libraries("${COMPONENT_TARGET}" PRIVATE "${req_interface}")
    endforeach()

    # Signal to idf_component_include that this component was included via the
    # backward compatible idf_component_register function.
    idf_component_set_property("${COMPONENT_NAME}" COMPONENT_FORMAT CMAKEV1)

    # The handling of WHOLE_ARCHIVE linkage is managed within the
    # idf_component_include function.
    idf_component_set_property("${COMPONENT_NAME}" WHOLE_ARCHIVE "${ARG_WHOLE_ARCHIVE}")

    idf_component_set_property("${COMPONENT_NAME}" SRCS "${sources}")
    idf_component_set_property("${COMPONENT_NAME}" INCLUDE_DIRS "${ARG_INCLUDE_DIRS}")
    idf_component_set_property("${COMPONENT_NAME}" PRIV_INCLUDE_DIRS "${ARG_PRIV_INCLUDE_DIRS}")

    # The addition of ldgen fragment files is managed by the
    # idf_component_include function.
    idf_component_set_property("${COMPONENT_NAME}" LDFRAGMENTS "${ARG_LDFRAGMENTS}")

    # Embedded files are managed in the idf_component_include function.
    idf_component_set_property("${COMPONENT_NAME}" EMBED_FILES "${ARG_EMBED_FILES}")
    idf_component_set_property("${COMPONENT_NAME}" EMBED_TXTFILES "${ARG_EMBED_TXTFILES}")
    idf_component_set_property("${COMPONENT_NAME}" REQUIRES "${ARG_REQUIRES}")
    idf_component_set_property("${COMPONENT_NAME}" PRIV_REQUIRES "${ARG_PRIV_REQUIRES}")
    idf_component_set_property("${COMPONENT_NAME}" REQUIRED_IDF_TARGETS "${ARG_REQUIRED_IDF_TARGETS}")
    idf_component_set_property("${COMPONENT_NAME}" COMPONENT_TYPE "${component_type}")
endfunction()

#[[
.. cmakev2:function:: idf_build_component

    .. code-block:: cmake

        idf_build_component(<component_dir> [<component_source>])

    *component_dir[in]*

        Directory path of the component.

    *component_source[in,opt]*

        Source of the component. One of:

        * ``idf_components``
        * ``project_managed_components``
        * ``project_extra_components``
        * ``project_components``

        Defaults to ``project_components``.

    Compatibility shim used by generated files (e.g. managed_components_list)
    that initializes the component in build system v2 by delegating to
    ``__init_component``.
#]]
function(idf_build_component component_dir)
    if(${ARGC} EQUAL 1)
        set(component_source "project_components")
    else()
        set(component_source ${ARGV1})
    endif()

    # Get the component name and check if it's already initialized
    get_filename_component(component_name "${component_dir}" NAME)
    __get_component_interface(COMPONENT "${component_name}" OUTPUT existing_interface)

    if(existing_interface)
        # Component already exists, check if it's from the same directory and source
        idf_component_get_property(existing_dir "${existing_interface}" COMPONENT_DIR)
        idf_component_get_property(existing_source "${existing_interface}" COMPONENT_SOURCE)

        # If same directory and same source, skip re-initialization
        # This handles the case where component manager may output the same component
        # multiple times due to path separator issues on Windows.
        # TODO: Remove this workaround once idf-component-manager is fixed (IDF-14260)
        if("${component_dir}" STREQUAL "${existing_dir}" AND
           "${component_source}" STREQUAL "${existing_source}")
            # Component already initialized from the same location, skip duplicate
            return()
        endif()

        # Different directory or source, let __init_component handle the priority logic
    endif()

    idf_build_get_property(component_prefix PREFIX)
    __init_component(DIRECTORY "${component_dir}"
                     PREFIX "${component_prefix}"
                     SOURCE "${component_source}")
endfunction()
