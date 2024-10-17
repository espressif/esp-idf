idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_inc)

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    set(version_folder "hw_ver${CONFIG_SOC_TOUCH_SENSOR_VERSION}")
    if(CONFIG_SOC_TOUCH_SENSOR_VERSION GREATER 1)
        list(APPEND srcs "common/touch_sens_common.c"
                         "${version_folder}/touch_version_specific.c")
        list(APPEND public_inc  "include" "${version_folder}/include")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                    PRIV_REQUIRES esp_driver_gpio
                    INCLUDE_DIRS ${public_inc}
                    )
