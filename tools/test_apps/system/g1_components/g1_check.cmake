# Closure and dependency checks shared by the cmakev1 (g1_components) and
# cmakev2 (g1_components_cmakev2) variants of this test app. Must be included after
# the project has been configured.

set(extra_allowed_components
    ${CONFIG_IDF_TARGET_ARCH}
)

# These components are currently included into "G1" build, but shouldn't.
# After removing the extra dependencies, remove the components from this list as well.
set(extra_components_which_shouldnt_be_included
    # bootloader is only included from esptool_py, which should be removed from G1
    bootloader

    # bootloader_support is a dependency of the following G1 components:
    #    esp_system, esp_hw_support, esp_mspi.
    # as well as the following non G1 components:
    #    efuse, app_update, esp_partition
    # Challenging to remove: IDF-8581 for more details
    bootloader_support

    # should cxx be in G1? Can it exist without FreeRTOS? IDF-9511
    cxx

    # esp_driver_gpio is a dependency of esp_pm (should be removed from g1 builds),
    # as well as esp_hw_support, IDF-10387
    esp_driver_gpio

    # esp_app_format is dependency of bootloader_support, app_update, efuse.
    # All components that should be removed from G1
    esp_app_format

    # esp_bootloader_format is dependency of bootloader_support, app_update
    # All components that should be removed from G1
    esp_bootloader_format

    # Dependency of bootloader_support, app_update, and esp_hw_support
    #   Figure out if the esp_hw_support component can exist without a dependency on efuse.
    #   efuse is used by the ADC calibration functions in esp_hw_support/adc_share_hw_ctrl.c,
    #   it could use the efuse hal (if virtual efuse mode is not used for tests).
    #   If not, see if esp_hw_support can provide minimal efuse component replacement in G1 build.
    # Also used by security features (hmac and key-manager) and MAC support
    efuse

    # esp_pm is pulled in by esp_system due to pm_init and freertos idle hook
    # both could be moved to pm component if esp-system idle hook provided a way to register hooks
    # It is also used by esp_driver_gpio, which should be removed from G1-only build.
    # IDF-10415
    esp_pm

    # esp_timer is a dependency of esp_pm, esp_system, esp_security, esp_hw_support
    # esp_pm should be removed from G1 build
    # esp_system's dependency is due to task_wdt timer implementation on C2,
    # we could possibly place this implementation in esp_timer instead
    # esp_security uses it for esp_ds (used for timeout functionality)
    # and for componensating time after sleep (dependency could be reversed) IDF-10416
    esp_timer

    # esptool_py is a dependency of bootloader, app_update, partition_table, all of which
    #   should be removed from G1-only build.
    esptool_py

    # partition_table is pulled in by app_update, esptool_py, bootloader, esp_partition; all to be removed
    partition_table

    # esp_partition is a new component for separated IDF partition APIs. Pulled in from app_update and efuse,
    # both which should be removed
    esp_partition

    # esp_blockdev is a new component for separated IDF block storage device APIs. Pulled in from esp_partition
    esp_blockdev

    # pthread is required by cxx. See [refactor-todo] about cxx, can it work without pthread?
    pthread

    # esp_security is required by spi_flash
    esp_security

    # spi_flash is pulled in by bootloader_support (non-bootloader build).
    spi_flash
)

if(NOT IDF_BUILD_V2)
    # esp_usb_cdc_rom_console is used by the panic handler. Under build system v1
    # requirements are resolved before Kconfig, so esp_system has to depend on it
    # unconditionally. Build system v2 only pulls it in when the console is set to
    # USB CDC, which this app does not do.
    list(APPEND extra_components_which_shouldnt_be_included esp_usb_cdc_rom_console)
endif()

set(expected_components
    ${COMPONENTS}
    ${extra_allowed_components}
    ${extra_components_which_shouldnt_be_included}
)

if(IDF_BUILD_V2)
    # Build system v1 puts every component listed in COMPONENTS into the build, even
    # when nothing depends on it. Under v2 the closure is whatever actually gets
    # linked, so HAL components that only a single target pulls in are absent on all
    # the others. Keep these conditions in sync with the components pulling them in.
    idf_build_get_property(idf_target IDF_TARGET)

    if(NOT idf_target STREQUAL "esp32p4")
        # esp_hw_support only uses the CAM HAL for mipi_csi_share_hw_ctrl.c
        list(REMOVE_ITEM expected_components esp_hal_cam)
    endif()

    if(NOT idf_target STREQUAL "esp32")
        # esp_hal_ana_conv only uses the I2S HAL for the ESP32 SAR ADC
        list(REMOVE_ITEM expected_components esp_hal_i2s)
    endif()
endif()

list(SORT expected_components)

if(IDF_BUILD_V2)
    get_target_property(library ${project_elf} LIBRARY_INTERFACE)
    idf_library_get_property(build_components "${library}" LIBRARY_COMPONENTS_LINKED)
else()
    idf_build_get_property(build_components BUILD_COMPONENTS)
endif()
list(SORT build_components)

if(NOT "${expected_components}" STREQUAL "${build_components}")
    message(FATAL_ERROR "Unexpected components list in G1 build. "
                        "Expected: ${expected_components}. "
                        "Actual: ${build_components}")
endif()

set(comp_deps_dot "${CMAKE_BINARY_DIR}/component_deps.dot")
idf_build_get_property(target IDF_TARGET)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Checking dependency violations"
    COMMAND python "${CMAKE_CURRENT_LIST_DIR}/check_dependencies.py"
            --component_deps_file ${comp_deps_dot}
            --target ${IDF_TARGET}
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Found unexpected componend dependencies while running check_dependencies.py, "
                        "please update the dependency list in the script according to the error output.")
endif()
