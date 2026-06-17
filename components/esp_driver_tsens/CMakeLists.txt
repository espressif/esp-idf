idf_build_get_property(target IDF_TARGET)

set(srcs)
set(priv_req efuse)
set(public_include "include")
if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "src/temperature_sensor.c")
    if(CONFIG_SOC_TEMPERATURE_SENSOR_SUPPORT_ETM)
        list(APPEND srcs "src/temperature_sensor_etm.c")
    endif()
    if(CONFIG_SOC_PAU_SUPPORTED AND CONFIG_SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION
       AND CONFIG_SOC_TEMPERATURE_SENSOR_UNDER_PD_TOP_DOMAIN)
        list(APPEND srcs "${target}/temperature_sensor_retention.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_INCLUDE_DIRS "src"
                       REQUIRES esp_hal_ana_conv
                       PRIV_REQUIRES ${priv_req}
                      )
