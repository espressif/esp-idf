idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_inc)
set(priv_require esp_driver_gpio)

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    set(version_folder "hw_ver${CONFIG_SOC_TOUCH_SENSOR_VERSION}")
    list(APPEND srcs "common/touch_sens_common.c"
                        "${version_folder}/touch_version_specific.c")
    list(APPEND public_inc  "include" "${version_folder}/include")
endif()

# ESP32 need esp_timer to execute the software filter periodically
if(${target} STREQUAL "esp32")
    list(APPEND priv_require esp_timer)
endif()

idf_component_register(SRCS ${srcs}
                    PRIV_REQUIRES ${priv_require}
                    INCLUDE_DIRS ${public_inc}
                    )
