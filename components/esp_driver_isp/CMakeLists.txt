set(srcs)

set(public_include "include")

if(CONFIG_SOC_ISP_SUPPORTED)
    list(APPEND srcs "src/isp_core.c"
                     "src/isp_af.c")
endif()

if(CONFIG_SOC_ISP_BF_SUPPORTED)
    list(APPEND srcs "src/isp_bf.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES esp_driver_gpio
                      )
