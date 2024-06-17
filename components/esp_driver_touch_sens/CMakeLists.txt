idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_inc)

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    if(CONFIG_SOC_TOUCH_SENSOR_VERSION EQUAL 3)
        list(APPEND srcs "hw_ver3/touch_version_specific.c"
                         "common/touch_sens_common.c")
        list(APPEND public_inc  "include" "hw_ver3/include")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                    PRIV_REQUIRES esp_driver_gpio
                    INCLUDE_DIRS ${public_inc}
                    )
