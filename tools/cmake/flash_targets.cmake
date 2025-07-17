# flash_targets.cmake
#
# Functions for setting up flash-related targets and configuration

#
# Setup flash targets for the application
#
function(__idf_build_setup_flash_targets)
    # Set up app-flash and flash targets. The app-flash target is specifically for flashing
    # just the application, while the flash target is for flashing the entire system.
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(project_bin PROJECT_BIN)
    partition_table_get_partition_info(app_partition_offset "--partition-boot-default" "offset")
    esptool_py_custom_target(app-flash app "app")

    esptool_py_flash_target_image(app-flash app "${app_partition_offset}" "${build_dir}/${project_bin}")
    esptool_py_flash_target_image(flash app "${app_partition_offset}" "${build_dir}/${project_bin}")

    # Setup the main flash target and dependencies
    __esptool_py_setup_main_flash_target()

    # Generate flasher_args.json configuration files
    __idf_build_generate_flasher_args()
endfunction()

#
# Generate flasher_args.json configuration files
#
function(__idf_build_generate_flasher_args)
    # Generate flasher_args.json for tools that need it. The variables below are used
    # in configuring the template flasher_args.json.in.
    # Some of the variables (flash mode, size, frequency) are set as esptool_py component's properties.

    idf_build_get_property(target IDF_TARGET)
    set(ESPTOOLPY_CHIP "${target}")
    set(ESPTOOLPY_BEFORE "${CONFIG_ESPTOOLPY_BEFORE}")
    set(ESPTOOLPY_AFTER  "${CONFIG_ESPTOOLPY_AFTER}")
    if(CONFIG_ESPTOOLPY_NO_STUB)
        set(ESPTOOLPY_WITH_STUB false)
    else()
        set(ESPTOOLPY_WITH_STUB true)
    endif()

    if(CONFIG_SECURE_BOOT OR CONFIG_SECURE_FLASH_ENC_ENABLED)
        # If security enabled then override post flash option
        set(ESPTOOLPY_AFTER "no_reset")
    endif()

    idf_component_get_property(ESPFLASHMODE esptool_py ESPFLASHMODE)
    idf_component_get_property(ESPFLASHFREQ esptool_py ESPFLASHFREQ)
    idf_component_get_property(ESPFLASHSIZE esptool_py ESPFLASHSIZE)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

    # Generate flasher args files
    idf_build_get_property(build_dir BUILD_DIR)
    file(READ "${esptool_py_dir}/flasher_args.json.in" flasher_args_content)
    string(CONFIGURE "${flasher_args_content}" flasher_args_content)

    # We need to create a flasher_args.json.in to create the final flasher_args.json
    # because CMake only resolves generator expressions in the file_generate command
    # with the INPUT keyword during the generation phase.
    file_generate("${build_dir}/flasher_args.json.in"
                 CONTENT "${flasher_args_content}")
    file_generate("${build_dir}/flasher_args.json"
                 INPUT "${build_dir}/flasher_args.json.in")
endfunction()
