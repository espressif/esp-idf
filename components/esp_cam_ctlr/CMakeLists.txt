set(srcs "esp_cam_ctlr.c")

set(include "include" "interface")

if(CONFIG_SOC_MIPI_CSI_SUPPORTED)
    list(APPEND srcs "csi/src/esp_cam_ctlr_csi.c")
    list(APPEND include "csi/include")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       PRIV_REQUIRES esp_mm
                      )
