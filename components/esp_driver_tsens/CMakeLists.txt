set(srcs)
set(priv_req efuse)
set(public_include "include")
if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "src/temperature_sensor.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES ${priv_req}
                      )
