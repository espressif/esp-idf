# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

set(g0_components soc hal esp_common esp_rom)  # also <arch>, i.e. xtensa or riscv, will be added below
set(g1_components spi_flash freertos log heap esp_libc esp_system esp_hw_support esp_mm esp_stdio)
# The HAL components that required by G1 components
# Most of these HAL components are required by `clk.c` in `esp_system` for peripherals clock initialization.
set(esp_hal_components
    esp_hal_ana_conv
    esp_hal_cam
    esp_hal_dma
    esp_hal_gpio
    esp_hal_i2s
    esp_hal_mspi
    esp_hal_gpspi
    esp_hal_timg
    esp_hal_touch_sens
    esp_hal_uart
    esp_hal_usb
    esp_hal_wdt
    esp_hal_pmu
    esp_hal_rtc_timer
    esp_hal_clock
)
set(COMPONENTS ${g0_components} ${g1_components} ${esp_hal_components} main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

project(g1_components)

set(extra_allowed_components
    ${CONFIG_IDF_TARGET_ARCH}
)

# These components are currently included into "G1" build, but shouldn't.
# After removing the extra dependencies, remove the components from this list as well.
set(extra_components_which_shouldnt_be_included
    # app_update gets added because of spi_flash and esp_partition.
    # esp_partition will get removed from g1 and spi_flash does not actually seem to need app_update anymore.
    # When esp-partition is removed from g1 build it should be easy for us to also remove app-update # TODO IDF-8577
    app_update

    # bootloader is only included from esptool_py, which should be removed from G1
    bootloader

    # bootloader_support is a dependency of the following G1 components:
    #    spi_flash, esp_system, esp_hw_support.
    # as well as the following non G1 components:
    #    efuse, app_update, esp_partition
    # Challenging to remove: IDF-8581 for more details
    bootloader_support

    # should cxx be in G1? Can it exist without FreeRTOS? IDF-9511
    cxx

    # esp_driver_gpio is a dependency of esp_pm (should be removed from g1 builds),
    # as well as spi_flash, esp_hw_support, IDF-10387
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
    # esp_hw_support dependency seems like it could be removed?
    # It is also used by esp_driver_gpio, mbedtls all of which should be removed from G1-only build.
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

    # mbedtls is a dependency of bootloader_support (plus other easier-to-remove ones)
    #   it is hard to make it conditional, need to remove bootloader_support.
    mbedtls

    # partition_table is pulled in by app_update, esptool_py, bootloader, esp_partition; all to be removed
    partition_table

    # esp_partition is a new component for separated IDF partition APIs. Pulled in from app_update and efuse,
    # both which should be removed
    esp_partition

    # esp_blockdev is a new component for separated IDF block storage device APIs. Pulled in from esp_partition
    esp_blockdev

    # pthread is required by cxx. See [refactor-todo] about cxx, can it work without pthread?
    pthread

    # esp_security is required by mbedtls and spi_flash
    esp_security

    # esp_usb_cdc_rom_console is used by the panic handler, will be conditional on cdc console option when
    # the new build-system is implemented
    esp_usb_cdc_rom_console
)

set(expected_components
    ${COMPONENTS}
    ${extra_allowed_components}
    ${extra_components_which_shouldnt_be_included}
)

list(SORT expected_components)


idf_build_get_property(build_components BUILD_COMPONENTS)
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
    COMMAND python "${CMAKE_SOURCE_DIR}/check_dependencies.py"
            --component_deps_file ${comp_deps_dot}
            --target ${IDF_TARGET}
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Found unexpected componend dependencies while running check_dependencies.py, "
                        "please update the dependency list in the script according to the error output.")
endif()
