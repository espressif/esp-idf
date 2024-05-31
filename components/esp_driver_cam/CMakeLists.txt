idf_build_get_property(target IDF_TARGET)

set(srcs "esp_cam_ctlr.c")

set(include "include" "interface")

if(CONFIG_SOC_MIPI_CSI_SUPPORTED)
    list(APPEND srcs "csi/src/esp_cam_ctlr_csi.c")
    list(APPEND include "csi/include")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_mm)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       PRIV_REQUIRES "${priv_requires}"
                      )
