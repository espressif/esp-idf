
# idf_build_get_property
#
# @brief Retrieve the value of the specified property related to ESP-IDF build.
#
# @param[out] var the variable to store the value in
# @param[in] property the property to get the value of
#
# @param[in, optional] GENERATOR_EXPRESSION (option) retrieve the generator expression for the property
#                   instead of actual value
function(idf_build_get_property var property)
    cmake_parse_arguments(_ "GENERATOR_EXPRESSION" "" "" ${ARGN})
    if(__GENERATOR_EXPRESSION)
        set(val "$<TARGET_PROPERTY:__idf_build_target,${property}>")
    else()
        get_property(val TARGET __idf_build_target PROPERTY ${property})
    endif()
    set(${var} ${val} PARENT_SCOPE)
endfunction()

# idf_build_set_property
#
# @brief Set the value of the specified property related to ESP-IDF build. The property is
#        also added to the internal list of build properties if it isn't there already.
#
# @param[in] property the property to set the value of
# @param[out] value value of the property
#
# @param[in, optional] APPEND (option) append the value to the current value of the
#                     property instead of replacing it
function(idf_build_set_property property value)
    cmake_parse_arguments(_ "APPEND" "" "" ${ARGN})

    if(__APPEND)
        set_property(TARGET __idf_build_target APPEND PROPERTY ${property} ${value})
    else()
        set_property(TARGET __idf_build_target PROPERTY ${property} ${value})
    endif()

    # Keep track of set build properties so that they can be exported to a file that
    # will be included in early expansion script.
    idf_build_get_property(build_properties __BUILD_PROPERTIES)
    if(NOT property IN_LIST build_properties)
        idf_build_set_property(__BUILD_PROPERTIES "${property}" APPEND)
    endif()
endfunction()

# idf_build_unset_property
#
# @brief Unset the value of the specified property related to ESP-IDF build. Equivalent
#        to setting the property to an empty string; though it also removes the property
#        from the internal list of build properties.
#
# @param[in] property the property to unset the value of
function(idf_build_unset_property property)
    idf_build_set_property(${property} "") # set to an empty value
    idf_build_get_property(build_properties __BUILD_PROPERTIES) # remove from tracked properties
    list(REMOVE_ITEM build_properties ${property})
    idf_build_set_property(__BUILD_PROPERTIES "${build_properties}")
endfunction()

#
# Retrieve the IDF_PATH repository's version, either using a version
# file or Git revision. Sets the IDF_VER build property.
#
function(__build_get_idf_git_revision)
    idf_build_get_property(idf_path IDF_PATH)
    git_describe(idf_ver_git "${idf_path}")
    if(EXISTS "${idf_path}/version.txt")
        file(STRINGS "${idf_path}/version.txt" idf_ver_t)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${idf_path}/version.txt")
    else()
        set(idf_ver_t ${idf_ver_git})
    endif()
    # cut IDF_VER to required 32 characters.
    string(SUBSTRING "${idf_ver_t}" 0 31 idf_ver)
    idf_build_set_property(COMPILE_DEFINITIONS "-DIDF_VER=\"${idf_ver}\"" APPEND)
    git_submodule_check("${idf_path}")
    idf_build_set_property(IDF_VER ${idf_ver})
endfunction()

#
# Sets initial list of build specifications (compile options, definitions, etc.) common across
# all library targets built under the ESP-IDF build system. These build specifications are added
# privately using the directory-level CMake commands (add_compile_options, include_directories, etc.)
# during component registration.
#
function(__build_set_default_build_specifications)
    unset(compile_definitions)
    unset(compile_options)
    unset(c_compile_options)
    unset(cxx_compile_options)

    list(APPEND compile_definitions "-D_GNU_SOURCE")

    list(APPEND compile_options     "-ffunction-sections"
                                    "-fdata-sections"
                                    "-fstrict-volatile-bitfields"
                                    "-nostdlib"
                                    # warning-related flags
                                    "-Wall"
                                    "-Werror=all"
                                    "-Wno-error=unused-function"
                                    "-Wno-error=unused-but-set-variable"
                                    "-Wno-error=unused-variable"
                                    "-Wno-error=deprecated-declarations"
                                    "-Wextra"
                                    "-Wno-unused-parameter"
                                    "-Wno-sign-compare"
                                    # always generate debug symbols (even in release mode, these don't
                                    # go into the final binary so have no impact on size
                                    "-ggdb")

    list(APPEND c_compile_options   "-std=gnu99"
                                    "-Wno-old-style-declaration")

    list(APPEND cxx_compile_options "-std=gnu++11")

    idf_build_set_property(COMPILE_DEFINITIONS "${compile_definitions}" APPEND)
    idf_build_set_property(COMPILE_OPTIONS "${compile_options}" APPEND)
    idf_build_set_property(C_COMPILE_OPTIONS "${c_compile_options}" APPEND)
    idf_build_set_property(CXX_COMPILE_OPTIONS "${cxx_compile_options}" APPEND)
endfunction()

#
# Initialize the build. This gets called upon inclusion of idf.cmake to set internal
# properties used for the processing phase of the build.
#
function(__build_init idf_path)
    # Create the build target, to which the ESP-IDF build properties, dependencies are attached to
    add_library(__idf_build_target STATIC IMPORTED)

    set_default(python "python")

    idf_build_set_property(PYTHON ${python})
    idf_build_set_property(IDF_PATH ${idf_path})

    idf_build_set_property(__PREFIX idf)
    idf_build_set_property(__CHECK_PYTHON 1)

    __build_set_default_build_specifications()

    # Add internal components to the build
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(prefix __PREFIX)
    file(GLOB component_dirs ${idf_path}/components/*)
    foreach(component_dir ${component_dirs})
        get_filename_component(component_dir ${component_dir} ABSOLUTE)
        __component_dir_quick_check(is_component ${component_dir})
        if(is_component)
            __component_add(${component_dir} ${prefix})
        endif()
    endforeach()

    # Set components required by all other components in the build
    set(requires_common cxx newlib freertos heap log soc esp_rom esp_common xtensa)
    idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${requires_common}")

    __build_get_idf_git_revision()
    __kconfig_init()
endfunction()

# idf_build_component
#
# @brief Present a directory that contains a component to the build system.
#        Relative paths are converted to absolute paths with respect to current directory.
#        All calls to this command must be performed before idf_build_process.
#
# @note  This command does not guarantee that the component will be processed
#        during build (see the COMPONENTS argument description for command idf_build_process)
#
# @param[in] component_dir directory of the component
function(idf_build_component component_dir)
    idf_build_get_property(prefix __PREFIX)
    __component_add(${component_dir} ${prefix} 0)
endfunction()

#
# Resolve the requirement component to the component target created for that component.
#
function(__build_resolve_and_add_req var component_target req type)
    __component_get_target(_component_target ${req})
    __component_get_property(_component_registered ${component_target} __COMPONENT_REGISTERED)
    if(NOT _component_target OR NOT _component_registered)
        message(FATAL_ERROR "Failed to resolve component '${req}'.")
    endif()
    __component_set_property(${component_target} ${type} ${_component_target} APPEND)
    set(${var} ${_component_target} PARENT_SCOPE)
endfunction()

#
# Build a list of components (in the form of component targets) to be added to the build
# based on public and private requirements. This list is saved in an internal property,
# __BUILD_COMPONENT_TARGETS.
#
function(__build_expand_requirements component_target)
    # Since there are circular dependencies, make sure that we do not infinitely
    # expand requirements for each component.
    idf_build_get_property(component_targets_seen __COMPONENT_TARGETS_SEEN)
    __component_get_property(component_registered ${component_target} __COMPONENT_REGISTERED)
    if(component_target IN_LIST component_targets_seen OR NOT component_registered)
        return()
    endif()

    idf_build_set_property(__COMPONENT_TARGETS_SEEN ${component_target} APPEND)

    get_property(reqs TARGET ${component_target} PROPERTY REQUIRES)
    get_property(priv_reqs TARGET ${component_target} PROPERTY PRIV_REQUIRES)

    foreach(req ${reqs})
        __build_resolve_and_add_req(_component_target ${component_target} ${req} __REQUIRES)
        __build_expand_requirements(${_component_target})
    endforeach()

    foreach(req ${priv_reqs})
        __build_resolve_and_add_req(_component_target ${component_target} ${req} __PRIV_REQUIRES)
        __build_expand_requirements(${_component_target})
    endforeach()

    idf_build_get_property(build_component_targets __BUILD_COMPONENT_TARGETS)
    if(NOT component_target IN_LIST build_component_targets)
        idf_build_set_property(__BUILD_COMPONENT_TARGETS ${component_target} APPEND)

        __component_get_property(component_lib ${component_target} COMPONENT_LIB)
        idf_build_set_property(__BUILD_COMPONENTS ${component_lib} APPEND)

        idf_build_get_property(prefix __PREFIX)
        __component_get_property(component_prefix ${component_target} __PREFIX)

        __component_get_property(component_alias ${component_target} COMPONENT_ALIAS)

        idf_build_set_property(BUILD_COMPONENT_ALIASES ${component_alias} APPEND)

        # Only put in the prefix in the name if it is not the default one
        if(component_prefix STREQUAL prefix)
            __component_get_property(component_name ${component_target} COMPONENT_NAME)
            idf_build_set_property(BUILD_COMPONENTS ${component_name} APPEND)
        else()
            idf_build_set_property(BUILD_COMPONENTS ${component_alias} APPEND)
        endif()
    endif()
endfunction()

#
# Write a CMake file containing set build properties, owing to the fact that an internal
# list of properties is maintained in idf_build_set_property() call. This is used to convert
# those set properties to variables in the scope the output file is included in.
#
function(__build_write_properties output_file)
    idf_build_get_property(build_properties __BUILD_PROPERTIES)
    foreach(property ${build_properties})
        idf_build_get_property(val ${property})
        set(build_properties_text "${build_properties_text}\nset(${property} ${val})")
    endforeach()
    file(WRITE ${output_file} "${build_properties_text}")
endfunction()

#
# Check if the Python interpreter used for the build has all the required modules.
#
function(__build_check_python)
    idf_build_get_property(check __CHECK_PYTHON)
    if(check)
        idf_build_get_property(python PYTHON)
        idf_build_get_property(idf_path IDF_PATH)
        message(STATUS "Checking Python dependencies...")
        execute_process(COMMAND "${python}" "${idf_path}/tools/check_python_dependencies.py"
            RESULT_VARIABLE result)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "Some Python dependencies must be installed. Check above message for details.")
        endif()
    endif()
endfunction()

#
# Prepare for component processing expanding each component's project include
#
macro(__build_process_project_includes)
    # Include the sdkconfig cmake file, since the following operations require
    # knowledge of config values.
    idf_build_get_property(sdkconfig_cmake SDKCONFIG_CMAKE)
    include(${sdkconfig_cmake})

    # Make each build property available as a read-only variable
    idf_build_get_property(build_properties __BUILD_PROPERTIES)
    foreach(build_property ${build_properties})
        idf_build_get_property(val ${build_property})
        set(${build_property} "${val}")
    endforeach()

    # Check that the CMake target value matches the Kconfig target value.
    __target_check()

    idf_build_get_property(build_component_targets __BUILD_COMPONENT_TARGETS)

    # Include each component's project_include.cmake
    foreach(component_target ${build_component_targets})
        __component_get_property(dir ${component_target} COMPONENT_DIR)
        __component_get_property(_name ${component_target} COMPONENT_NAME)
        set(COMPONENT_NAME ${_name})
        set(COMPONENT_DIR ${dir})
        set(COMPONENT_PATH ${dir})  # this is deprecated, users are encouraged to use COMPONENT_DIR;
                                    # retained for compatibility
        if(EXISTS ${COMPONENT_DIR}/project_include.cmake)
            include(${COMPONENT_DIR}/project_include.cmake)
        endif()
    endforeach()
endmacro()

#
# Utility macro for setting default property value if argument is not specified
# for idf_build_process().
#
macro(__build_set_default var default)
    set(_var __${var})
    if(NOT "${_var}" STREQUAL "")
        idf_build_set_property(${var} "${${_var}}")
    else()
        idf_build_set_property(${var} "${default}")
    endif()
    unset(_var)
endmacro()

#
# Import configs as build instance properties so that they are accessible
# using idf_build_get_config(). Config has to have been generated before calling
# this command.
#
function(__build_import_configs)
    # Include the sdkconfig cmake file, since the following operations require
    # knowledge of config values.
    idf_build_get_property(sdkconfig_cmake SDKCONFIG_CMAKE)
    include(${sdkconfig_cmake})

    idf_build_set_property(__CONFIG_VARIABLES "${CONFIGS_LIST}")
    foreach(config ${CONFIGS_LIST})
        set_property(TARGET __idf_build_target PROPERTY ${config} "${${config}}")
    endforeach()
endfunction()

# idf_build_process
#
# @brief Main processing step for ESP-IDF build: config generation, adding components to the build,
#        dependency resolution, etc.
#
# @param[in] target ESP-IDF target
#
# @param[in, optional] PROJECT_DIR (single value) directory of the main project the buildsystem
#                      is processed for; defaults to CMAKE_SOURCE_DIR
# @param[in, optional] PROJECT_VER (single value) version string of the main project; defaults
#                      to 1
# @param[in, optional] PROJECT_NAME (single value) main project name, defaults to CMAKE_PROJECT_NAME
# @param[in, optional] SDKCONFIG (single value) sdkconfig output path, defaults to PROJECT_DIR/sdkconfig
#                       if PROJECT_DIR is set and CMAKE_SOURCE_DIR/sdkconfig if not
# @param[in, optional] SDKCONFIG_DEFAULTS (single value) config defaults file to use for the build; defaults
#                       to none (Kconfig defaults or previously generated config are used)
# @param[in, optional] BUILD_DIR (single value) directory for build artifacts; defautls to CMAKE_BINARY_DIR
# @param[in, optional] COMPONENTS (multivalue) select components to process among the components
#                       known by the build system
#                       (added via `idf_build_component`). This argument is used to trim the build.
#                       Other components are automatically added if they are required
#                       in the dependency chain, i.e.
#                       the public and private requirements of the components in this list
#                       are automatically added, and in
#                       turn the public and private requirements of those requirements,
#                       so on and so forth. If not specified, all components known to the build system
#                       are processed.
macro(idf_build_process target)
    set(options)
    set(single_value PROJECT_DIR PROJECT_VER PROJECT_NAME BUILD_DIR SDKCONFIG SDKCONFIG_DEFAULTS)
    set(multi_value COMPONENTS)
    cmake_parse_arguments(_ "${options}" "${single_value}" "${multi_value}" ${ARGN})

    idf_build_set_property(BOOTLOADER_BUILD "${BOOTLOADER_BUILD}")

    # Check build target is specified. Since this target corresponds to a component
    # name, the target component is automatically added to the list of common component
    # requirements.
    if(target STREQUAL "")
        message(FATAL_ERROR "Build target not specified.")
    endif()

    idf_build_set_property(IDF_TARGET ${target})

    __build_set_default(PROJECT_DIR ${CMAKE_SOURCE_DIR})
    __build_set_default(PROJECT_NAME ${CMAKE_PROJECT_NAME})
    __build_set_default(PROJECT_VER 1)
    __build_set_default(BUILD_DIR ${CMAKE_BINARY_DIR})

    idf_build_get_property(project_dir PROJECT_DIR)
    __build_set_default(SDKCONFIG "${project_dir}/sdkconfig")

    __build_set_default(SDKCONFIG_DEFAULTS "")

    # Check for required Python modules
    __build_check_python()

    idf_build_set_property(__COMPONENT_REQUIRES_COMMON ${target} APPEND)

    # Perform early expansion of component CMakeLists.txt in CMake scripting mode.
    # It is here we retrieve the public and private requirements of each component.
    # It is also here we add the common component requirements to each component's
    # own requirements.
    __component_get_requirements()

    idf_build_get_property(component_targets __COMPONENT_TARGETS)

    # Finally, do component expansion. In this case it simply means getting a final list
    # of build component targets given the requirements set by each component.

    # Check if we need to trim the components first, and build initial components list
    # from that.
    if(__COMPONENTS)
        unset(component_targets)
        foreach(component ${__COMPONENTS})
            __component_get_target(component_target ${component})
            if(NOT component_target)
                message(FATAL_ERROR "Failed to resolve component '${component}'.")
            endif()
            list(APPEND component_targets ${component_target})
        endforeach()
    endif()

    foreach(component_target ${component_targets})
        __build_expand_requirements(${component_target})
    endforeach()
    idf_build_unset_property(__COMPONENT_TARGETS_SEEN)

    # Get a list of common component requirements in component targets form (previously
    # we just have a list of component names)
    idf_build_get_property(common_reqs __COMPONENT_REQUIRES_COMMON)
    foreach(common_req ${common_reqs})
        __component_get_target(component_target ${common_req})
        __component_get_property(lib ${component_target} COMPONENT_LIB)
        idf_build_set_property(___COMPONENT_REQUIRES_COMMON ${lib} APPEND)
    endforeach()

    # Generate config values in different formats
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(sdkconfig_defaults SDKCONFIG_DEFAULTS)
    __kconfig_generate_config("${sdkconfig}" "${sdkconfig_defaults}")
    __build_import_configs()

    # Temporary trick to support both gcc5 and gcc8 builds
    if(CMAKE_C_COMPILER_VERSION VERSION_EQUAL 5.2.0)
        set(GCC_NOT_5_2_0 0 CACHE STRING "GCC is 5.2.0 version")
    else()
        set(GCC_NOT_5_2_0 1 CACHE STRING "GCC is not 5.2.0 version")
    endif()
    idf_build_set_property(COMPILE_DEFINITIONS "-DGCC_NOT_5_2_0" APPEND)

    # All targets built under this scope is with the ESP-IDF build system
    set(ESP_PLATFORM 1)
    idf_build_set_property(COMPILE_DEFINITIONS "-DESP_PLATFORM" APPEND)

    # Perform component processing (inclusion of project_include.cmake, adding component
    # subdirectories, creating library targets, linking libraries, etc.)
    __build_process_project_includes()

    idf_build_get_property(idf_path IDF_PATH)
    add_subdirectory(${idf_path} ${build_dir}/esp-idf)

    unset(ESP_PLATFORM)
endmacro()

# idf_build_executable
#
# @brief Specify the executable the build system can attach dependencies to (for generating
# files used for linking, targets which should execute before creating the specified executable,
# generating additional binary files, generating files related to flashing, etc.)
function(idf_build_executable elf)
    # Propagate link dependencies from component library targets to the executable
    idf_build_get_property(link_depends __LINK_DEPENDS)
    set_property(TARGET ${elf} APPEND PROPERTY LINK_DEPENDS "${link_depends}")

    # Set the EXECUTABLE_NAME and EXECUTABLE properties since there are generator expression
    # from components that depend on it
    get_filename_component(elf_name ${elf} NAME_WE)
    idf_build_set_property(EXECUTABLE_NAME ${elf_name})
    idf_build_set_property(EXECUTABLE ${elf})

    # Add dependency of the build target to the executable
    add_dependencies(${elf} __idf_build_target)
endfunction()

# idf_build_get_config
#
# @brief Get value of specified config variable
function(idf_build_get_config var config)
    cmake_parse_arguments(_ "GENERATOR_EXPRESSION" "" "" ${ARGN})
    if(__GENERATOR_EXPRESSION)
        set(val "$<TARGET_PROPERTY:__idf_build_target,${config}>")
    else()
        get_property(val TARGET __idf_build_target PROPERTY ${config})
    endif()
    set(${var} ${val} PARENT_SCOPE)
endfunction()