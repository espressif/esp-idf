idf_build_get_property(target IDF_TARGET)
set(priv_requires esp_system esp_driver_gpio esp_timer)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "pm_locks.c" "pm_trace.c" "pm_impl.c")
if(CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED)
    list(APPEND srcs "pm_c5_flash_freq_limit.c")
endif()

if(${target} STREQUAL "esp32c5")
    # pm_c5_flash_freq_limit.c needs spi_flash header when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled
    list(APPEND priv_requires spi_flash)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS linker.lf)
