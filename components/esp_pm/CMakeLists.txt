idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)

if(CONFIG_SOC_PM_SUPPORTED)
    list(APPEND srcs "pm_locks.c" "pm_trace.c" "pm_impl.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES esp_system esp_driver_gpio esp_timer
                       LDFRAGMENTS linker.lf)
