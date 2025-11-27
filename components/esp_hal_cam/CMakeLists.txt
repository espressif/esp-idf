idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND public_include "${target}/include")
endif()

# CAM related source files
if(CONFIG_SOC_LCDCAM_CAM_SUPPORTED)
    list(APPEND srcs "cam_hal.c")
    list(APPEND srcs "${target}/cam_periph.c")
endif()

# CSI related source files
if(CONFIG_SOC_MIPI_CSI_SUPPORTED)
    list(APPEND srcs "mipi_csi_hal.c")
    list(APPEND srcs "${target}/mipi_csi_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal)
