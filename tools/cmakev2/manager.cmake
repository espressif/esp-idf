# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)
include(build)
include(kconfig)

#[[
    __init_component_manager()

    Initialize component manager related build properties and defaults.
#]]
function(__init_component_manager)
    # Set IDF_COMPONENT_MANAGER build property to 1 if not explicitly set to 0
    # in the environment.
    __get_default_value(VARIABLE IDF_COMPONENT_MANAGER
                        DEFAULT 1
                        OUTPUT component_manager_env)
    if(component_manager_env STREQUAL "" OR NOT component_manager_env STREQUAL "0")
        idf_build_set_property(IDF_COMPONENT_MANAGER 1)
    endif()

    # Set IDF_COMPONENT_MANAGER_INTERFACE_VERSION.
    # Defaults to 4. Allow overriding via env/CMake.
    __get_default_value(VARIABLE IDF_COMPONENT_MANAGER_INTERFACE_VERSION
                        DEFAULT 4
                        OUTPUT cmgr_iface)
    idf_build_set_property(IDF_COMPONENT_MANAGER_INTERFACE_VERSION ${cmgr_iface})

    # Set DEPENDENCIES_LOCK if set by the user. Otherwise, use the
    # project directory and IDF_TARGET to determine the lock file path.
    # Note: This deviates from the build system v1 behavior where we allow
    # users to specify the lock file path via idf_build_set_property.
    idf_build_get_property(deps_lock_file DEPENDENCIES_LOCK)
    __get_default_value(VARIABLE DEPENDENCIES_LOCK
                        DEFAULT "${deps_lock_file}"
                        OUTPUT deps_lock_file)
    idf_build_set_property(DEPENDENCIES_LOCK "${deps_lock_file}")
endfunction()

#[[
    __fetch_components_from_registry()

    Iteratively run the component manager and Kconfig until stable or error
    out.  This routine allows 1 re-run if the manager fails with a missing
    kconfig option. This behavior is similar to the build system v1.

    This routine performs the following steps:
    1. Initialize the component manager.
    2. Run the component manager for all discovered components.
    3. Re-collect Kconfig and regenerate sdkconfig with managed components included.
    4. If the component manager run failed, error out.
#]]
function(__fetch_components_from_registry)
    # Initialize the component manager.
    __init_component_manager()

    # Iteratively run the component manager and Kconfig until stable or error out.
    set(__cmgr_round 0)
    while(TRUE)
        math(EXPR __cmgr_round "${__cmgr_round} + 1")
        idf_msg("Component manager round ${__cmgr_round}...")

        # Run the component manager for all discovered components
        __download_component_level_managed_components(RESULT cmgr_result)

        # Re-collect Kconfig and regenerate sdkconfig with managed components included
        __generate_sdkconfig()

        # If component manager run failed, use the failure result
        if(cmgr_result EQUAL 0)
            # If manager is disabled but manifests were detected, issue a warning
            __component_manager_warn_if_disabled_and_manifests_exist()
            break()
        elseif(cmgr_result EQUAL 10 AND __cmgr_round LESS 2)
            # We can retry once if the manager fails with a missing kconfig option
            continue()
        elseif(cmgr_result EQUAL 10)
            idf_die("Missing required kconfig option after retry.")
        else()
            idf_die("IDF Component Manager error: ${cmgr_result}")
        endif()
    endwhile()
endfunction()

#[[
    __download_managed_component(COMPONENTS_LIST_FILE <file>
                                 MANAGED_OUTPUT_FILE <file>
                                 RESULT <variable>)

    *COMPONENTS_LIST_FILE[in]*

        Path to the local components list file

    *MANAGED_OUTPUT_FILE[in]*

        Path where managed components CMake file will be written

    *RESULT[out]*

        Exit code returned by the manager. 0 success, 10 re-run.

    Utility function to run the component manager with a specific components
    list and generate managed components output.
#]]
function(__download_managed_component)
    set(options)
    set(one_value COMPONENTS_LIST_FILE MANAGED_OUTPUT_FILE RESULT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_COMPONENTS_LIST_FILE)
        idf_die("COMPONENTS_LIST_FILE option is required")
    endif()
    if(NOT DEFINED ARG_MANAGED_OUTPUT_FILE)
        idf_die("MANAGED_OUTPUT_FILE option is required")
    endif()
    if(NOT DEFINED ARG_RESULT)
        idf_die("RESULT option is required")
    endif()

    idf_build_get_property(idf_component_manager IDF_COMPONENT_MANAGER)
    if(NOT idf_component_manager EQUAL 1)
        set(${ARG_RESULT} 0 PARENT_SCOPE)
        return()
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(project_dir PROJECT_DIR)
    idf_build_get_property(component_manager_interface_version IDF_COMPONENT_MANAGER_INTERFACE_VERSION)
    idf_build_get_property(dependencies_lock_file DEPENDENCIES_LOCK)
    idf_build_get_property(sdkconfig_json __SDKCONFIG_JSON)

    # Invoke the component manager
    execute_process(COMMAND ${python}
        "-m"
        "idf_component_manager.prepare_components"
        "--project_dir=${project_dir}"
        "--lock_path=${dependencies_lock_file}"
        "--sdkconfig_json_file=${sdkconfig_json}"
        "--interface_version=${component_manager_interface_version}"
        "prepare_dependencies"
        "--local_components_list_file=${ARG_COMPONENTS_LIST_FILE}"
        "--managed_components_list_file=${ARG_MANAGED_OUTPUT_FILE}"
        RESULT_VARIABLE result
        ERROR_VARIABLE error)

    if(NOT result EQUAL 0)
        if(result EQUAL 10)
            idf_warn("Component manager requested a re-run: ${error}")
        else()
            idf_die("Component manager failed: ${error}")
        endif()
    endif()

    set(${ARG_RESULT} ${result} PARENT_SCOPE)
endfunction()

#[[
    __download_component_level_managed_components(RESULT <variable>)

    *RESULT[out]*

        Exit code returned by the manager. 0 success, 10 re-run.

    Download component-level managed components
#]]
function(__download_component_level_managed_components)
    set(options)
    set(one_value RESULT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_RESULT)
        idf_die("RESULT option is required")
    endif()

    # Set up temporary files for the manager
    idf_build_get_property(build_dir BUILD_DIR)
    set(managed_components_list_file ${build_dir}/managed_components_list.temp.cmake)
    set(local_components_list_file ${build_dir}/local_components_list.temp.yml)

    # Build local components list from discovered components
    set(__contents "components:\n")
    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
    foreach(interface ${component_interfaces})
        __idf_component_get_property_unchecked(name ${interface} COMPONENT_NAME)
        __idf_component_get_property_unchecked(dir ${interface} COMPONENT_DIR)
        set(__contents "${__contents}  - name: \"${name}\"\n    path: \"${dir}\"\n")
    endforeach()
    file(WRITE ${local_components_list_file} "${__contents}")

    # Invoke the component manager
    __download_managed_component(COMPONENTS_LIST_FILE "${local_components_list_file}"
                                MANAGED_OUTPUT_FILE "${managed_components_list_file}"
                                RESULT result)

    # Ensure the manager produced the list of managed components
    if(result EQUAL 0 AND NOT EXISTS ${managed_components_list_file})
        idf_die("Managed components list file not produced by the component manager: ${managed_components_list_file}")
    endif()

    # Initialize managed components by including the generated list
    # Include components even if result is 10 (missing kconfig) to allow kconfig regeneration
    if(result EQUAL 0 OR result EQUAL 10)
        include(${managed_components_list_file})
    else()
        idf_warn("Component manager returned unexpected result: ${result}. Managed components will not be included.")
    endif()

    # Clean up temporary files
    if(NOT DEFINED ENV{IDF_KEEP_CMANAGER_TEMP} OR NOT "$ENV{IDF_KEEP_CMANAGER_TEMP}" STREQUAL "1")
        file(REMOVE ${managed_components_list_file})
        file(REMOVE ${local_components_list_file})
    endif()

    set(${ARG_RESULT} ${result} PARENT_SCOPE)
endfunction()

#[[
    __component_manager_warn_if_disabled_and_manifests_exist()

    When the component manager is disabled, warn if any discovered component
    contains an idf_component.yml manifest.
#]]
function(__component_manager_warn_if_disabled_and_manifests_exist)
    idf_build_get_property(idf_component_manager IDF_COMPONENT_MANAGER)
    if(idf_component_manager EQUAL 1)
        return()
    endif()
    idf_build_get_property(with_manifests __COMPONENTS_WITH_MANIFESTS)
    if(with_manifests)
        string(REPLACE ";" "\n\t" with_lines "${with_manifests}")
        idf_warn(NOTICE "\"idf_component.yml\" file was found for components:\n\t
        ${with_lines}\nHowever, the component manager is not enabled.")
    endif()
endfunction()

#[[
    __component_set_property(target property value)

    Shim for setting component properties, primarily for use by the component
    manager in build system v2. This function only processes dependency-related
    properties(MANAGED_REQUIRES and MANAGED_PRIV_REQUIRES) produced by the
    component manager's injection file. Other properties are ignored to avoid
    interfering with the cmakev2 build flow. Target names with triple
    underscores are normalized.
#]]
function(__component_set_property target property value)
    # If the target has 3 underscores, remove all of them and normalize the target
    # This shim is only intended to process dependency-related properties produced
    # by the component manager injection file. Ignore unrelated properties to avoid
    # clobbering configuration already set by the cmakev2 build flow.
    string(REPLACE "___" "" target "${target}")

    # We only consume MANAGED_REQUIRES and MANAGED_PRIV_REQUIRES from the component manager.
    # The manager's REQUIRES/PRIV_REQUIRES output contains both original and resolved names,
    # which we don't want. We'll handle name resolution locally using our utility functions.
    if(property STREQUAL "MANAGED_REQUIRES")
        # Set the managed property for tracking
        idf_component_set_property("${target}" "${property}" "${value}")
        # Also append to the regular REQUIRES property
        idf_component_set_property("${target}" REQUIRES "${value}" APPEND)
    elseif(property STREQUAL "MANAGED_PRIV_REQUIRES")
        # Set the managed property for tracking
        idf_component_set_property("${target}" "${property}" "${value}")
        # Also append to the regular PRIV_REQUIRES property
        idf_component_set_property("${target}" PRIV_REQUIRES "${value}" APPEND)
    else()
        # Ignore REQUIRES, PRIV_REQUIRES, and other properties like INCLUDE_DIRS,
        # __COMPONENT_SOURCE, __COMPONENT_REGISTERED, etc.
    endif()
endfunction()

#[[
    __inject_requirements_for_component_from_manager(<component_name>)

    Managed dependency injection for a single component in build system v2.
    Calls the Component Manager to compute manifest-derived dependencies and
    updates the component's MANAGED_* properties.
#]]
function(__inject_requirements_for_component_from_manager component_name)
    # Skip if already injected
    idf_component_get_property(already_injected "${component_name}" __MANAGED_INJECTED)
    if(already_injected)
        return()
    endif()

    idf_dbg("Injecting requirements for component '${component_name}' from the component manager")

    idf_build_get_property(python PYTHON)
    idf_build_get_property(project_dir PROJECT_DIR)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(dependencies_lock_file DEPENDENCIES_LOCK)
    idf_build_get_property(sdkconfig_json __SDKCONFIG_JSON)
    idf_build_get_property(component_manager_interface_version IDF_COMPONENT_MANAGER_INTERFACE_VERSION)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(component_prefix PREFIX)
    idf_component_get_property(component_source "${component_name}" COMPONENT_SOURCE)
    idf_component_get_property(component_dir "${component_name}" COMPONENT_DIR)

    # The component manager will inject requirements for this component. To do this, it needs to files:
    #
    # 1. An input file which states the component's source type. This is a minimal build system v1-style file
    #    which contains the component's source type. To make the component manager happy, we create a file with
    #    shim __component_set_property(), which calls idf_component_set_property(). The component manager will
    #    modify this file by adding the component's requirements. TODO: Improve this.
    # 2. A file which lists the components with manifests. This file is created by the component manager,
    #    and is deleted after the component manager is done. This works for build system v1 where we provide
    #    a global list of components with manifests. However, for build system v2, we need to provide this file
    #    for each component. Hence, we create this file and place it in the build directory.
    set(out_file "${build_dir}/component_requires.${component_name}.temp.cmake")
    set(cmgr_target "___${component_prefix}_${component_name}")
    # We only provide component source to the component manager
    file(WRITE "${out_file}" "__component_set_property(${cmgr_target} __COMPONENT_SOURCE \"${component_source}\")\n")

    # Create components_with_manifests_list.temp file with only this component if it has a manifest
    set(components_with_manifests_file "${build_dir}/components_with_manifests_list.temp")
    if(EXISTS "${component_dir}/idf_component.yml")
        file(WRITE "${components_with_manifests_file}" "${component_dir}\n")
    else()
        file(WRITE "${components_with_manifests_file}" "")
    endif()

    # Call component manager to inject requirements
    execute_process(COMMAND ${python}
        "-m"
        "idf_component_manager.prepare_components"
        "--project_dir=${project_dir}"
        "--lock_path=${dependencies_lock_file}"
        "--sdkconfig_json_file=${sdkconfig_json}"
        "--interface_version=${component_manager_interface_version}"
        "inject_requirements"
        "--idf_path=${idf_path}"
        "--build_dir=${build_dir}"
        "--component_requires_file=${out_file}"
        RESULT_VARIABLE result
        ERROR_VARIABLE error)

    if(NOT result EQUAL 0)
        idf_die("Component manager requirements injection failed for '${component_name}': ${error}")
    endif()

    # Include the component manager's output
    if(EXISTS "${out_file}")
        include("${out_file}")
    endif()

    # Clean up temporary files
    if(NOT DEFINED ENV{IDF_KEEP_CMANAGER_TEMP} OR NOT "$ENV{IDF_KEEP_CMANAGER_TEMP}" STREQUAL "1")
        file(REMOVE "${out_file}")
        file(REMOVE "${components_with_manifests_file}")
    endif()

    idf_component_set_property("${component_name}" __MANAGED_INJECTED YES)
endfunction()
