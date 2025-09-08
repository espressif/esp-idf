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

    # Set DEPENDENCIES_LOCK from env/-D if provided. Otherwise, use the
    # project directory and IDF_TARGET to determine the lock file path.
    # Note: This deviates from the build system v1 behavior where we allow
    # users to specify the lock file path via idf_build_set_property.
    __get_default_value(VARIABLE DEPENDENCIES_LOCK
                        DEFAULT ""
                        OUTPUT deps_lock_file)
    idf_build_set_property(DEPENDENCIES_LOCK "${deps_lock_file}")
endfunction()

#[[
   __fetch_components_from_registry()

   Iteratively run the component manager and Kconfig until stable or error out.
   This routine allows 1 re-run if the manager fails with a missing kconfig
   option. This behavior is similar to the build system v1.

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

    # Auto-include all project-level managed components after component manager is done
    __include_project_level_managed_components()
endfunction()

#[[
   __download_managed_component(COMPONENTS_LIST_FILE <file>
                                MANAGED_OUTPUT_FILE <file>
                                RESULT <variable>)

   Utility function to run the component manager with a specific components list
   and generate managed components output.

   :COMPONENTS_LIST_FILE[in]: Path to the local components list file
   :MANAGED_OUTPUT_FILE[in]: Path where managed components CMake file will be written
   :RESULT[out]: Exit code returned by the manager. 0 success, 10 re-run.
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

   Download component-level managed components

   :RESULT[out]: Exit code returned by the manager. 0 success, 10 re-run.
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
    idf_build_get_property(component_names COMPONENTS_DISCOVERED)
    foreach(name ${component_names})
        # Respect EXCLUDE_COMPONENTS passed via -D
        set(__include 1)
        if(DEFINED EXCLUDE_COMPONENTS AND name IN_LIST EXCLUDE_COMPONENTS)
            set(__include 0)
        endif()
        if(__include)
            idf_component_get_property(dir ${name} COMPONENT_DIR)
            set(__contents "${__contents}  - name: \"${name}\"\n    path: \"${dir}\"\n")
        endif()
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
    if(result EQUAL 0)
        include(${managed_components_list_file})
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
