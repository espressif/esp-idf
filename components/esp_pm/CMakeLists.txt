idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(SRCS "pm_locks.c" "pm_trace.c" "pm_impl.c"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES esp_system esp_driver_gpio esp_timer
                       LDFRAGMENTS linker.lf)
