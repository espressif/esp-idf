idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes "include")

if(CONFIG_SOC_SPIRAM_XIP_SUPPORTED)
    list(APPEND includes xip_impl/include)
endif()

set(priv_requires heap spi_flash esp_mm esp_hal_mspi esp_hal_gpio)
if(${target} STREQUAL "esp32")
    list(APPEND priv_requires bootloader_support esp_driver_spi esp_driver_gpio)
endif()

set(srcs "system_layer/esp_psram_mspi.c")

if(CONFIG_SPIRAM)
    list(APPEND srcs "system_layer/esp_psram.c")

    if(${target} STREQUAL "esp32")
        list(APPEND srcs "esp32/esp_psram_extram_cache.c"
                         "esp32/esp_himem.c")
    endif()

    if(CONFIG_SPIRAM_MODE_QUAD)
        if(${target} STREQUAL "esp32" OR ${target} STREQUAL "esp32s2")
            list(APPEND srcs "${target}/esp_psram_impl_quad.c")
        endif()

    elseif(CONFIG_SPIRAM_MODE_OCT)
        list(APPEND srcs "${target}/esp_psram_impl_octal.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS device/include
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

if(CONFIG_SPIRAM)
    add_subdirectory(device)

    if(CONFIG_SOC_SPIRAM_XIP_SUPPORTED)
        add_subdirectory(xip_impl)
    endif()

endif()

# TODO IDF-14338
if(CONFIG_IDF_TOOLCHAIN_CLANG AND CONFIG_IDF_TARGET_ESP32 AND CONFIG_SPIRAM_CACHE_WORKAROUND AND NOT BOOTLOADER_BUILD)
    # Note: Adding as a PUBLIC compile option here causes this option to propagate to all
    # components that depend on esp_psram.
    #
    # To handle some corner cases, the same flag is set in project_include.cmake
    target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
    # also, make sure we link with this option so correct toolchain libs are pulled in
    target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
    # set strategy selected
    # note that we don't need to set link options as the library linked is independent of this
    if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_DUPLDST)
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=dupldst)
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=dupldst)
    endif()
    if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_MEMW)
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=memw)
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=memw)
    endif()
    if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_NOPS)
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=nops)
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=nops)
    endif()
endif()
