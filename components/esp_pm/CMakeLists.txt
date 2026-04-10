idf_build_get_property(target IDF_TARGET)
set(priv_requires esp_system esp_driver_gpio esp_timer esp_hal_uart)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)

if(CONFIG_SOC_PM_SUPPORTED)
    list(APPEND srcs "pm_locks.c" "pm_trace.c" "pm_impl.c")
endif()

if(CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED)
    list(APPEND srcs "pm_c5_flash_freq_limit.c")
endif()

if(${target} STREQUAL "esp32c5")
    # pm_c5_flash_freq_limit.c needs spi_flash header when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled
    list(APPEND priv_requires spi_flash)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS linker.lf)

if(CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED)
    # Force linking init_flash ESP_SYSTEM_INIT_FN to ensure it's not discarded by linker
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u pm_c5_flash_freq_limit_init_include_func")
endif()
