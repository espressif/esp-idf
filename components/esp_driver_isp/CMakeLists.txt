idf_build_get_property(target IDF_TARGET)

set(srcs)

set(public_include "include")

set(priv_requires "esp_driver_gpio")

set(requires)

if(CONFIG_SOC_ISP_SUPPORTED)
    list(APPEND srcs "src/isp_core.c"
                     "src/isp_af.c"
                     "src/isp_ccm.c"
<<<<<<< HEAD
                     "src/isp_awb.c")
=======
                     "src/isp_awb.c"
                     "src/isp_ae.c"
                     "src/isp_gamma.c"
                     "src/isp_hist.c")
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
endif()

if(CONFIG_SOC_ISP_BF_SUPPORTED)
    list(APPEND srcs "src/isp_bf.c")
endif()

<<<<<<< HEAD
=======
if(CONFIG_SOC_ISP_DEMOSAIC_SUPPORTED)
    list(APPEND srcs "src/isp_demosaic.c")
endif()

if(CONFIG_SOC_ISP_SHARPEN_SUPPORTED)
    list(APPEND srcs "src/isp_sharpen.c")
endif()

if(CONFIG_SOC_ISP_COLOR_SUPPORTED)
    list(APPEND srcs "src/isp_color.c")
endif()

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
if(NOT ${target} STREQUAL "linux")
    list(APPEND requires esp_mm)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_requires}
<<<<<<< HEAD
=======
                       LDFRAGMENTS "linker.lf"
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
                      )
