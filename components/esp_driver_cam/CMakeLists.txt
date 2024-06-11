idf_build_get_property(target IDF_TARGET)

set(srcs "esp_cam_ctlr.c" "dvp_share_ctrl.c")

set(includes "include" "interface")

set(requires "esp_driver_isp")

set(priv_requires "esp_driver_gpio")

if(CONFIG_SOC_MIPI_CSI_SUPPORTED)
    list(APPEND srcs "csi/src/esp_cam_ctlr_csi.c")
    list(APPEND includes "csi/include")
endif()

if(CONFIG_SOC_ISP_DVP_SUPPORTED)
    list(APPEND srcs "isp_dvp/src/esp_cam_ctlr_isp_dvp.c")
    list(APPEND includes "isp_dvp/include")
endif()

if(NOT ${target} STREQUAL "linux")
    list(APPEND requires esp_mm)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_requires}
                      )
