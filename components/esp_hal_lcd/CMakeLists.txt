idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND public_include "${target}/include")
endif()

if(CONFIG_SOC_LCDCAM_I80_LCD_SUPPORTED OR CONFIG_SOC_LCDCAM_RGB_LCD_SUPPORTED)
    list(APPEND srcs "lcd_hal.c")
endif()

if(CONFIG_SOC_MIPI_DSI_SUPPORTED)
    list(APPEND srcs "mipi_dsi_hal.c")
endif()

if(CONFIG_SOC_I2S_I80_LCD_SUPPORTED OR CONFIG_SOC_LCDCAM_I80_LCD_SUPPORTED OR CONFIG_SOC_LCDCAM_RGB_LCD_SUPPORTED)
    list(APPEND srcs "${target}/lcd_periph.c")
endif()

if(CONFIG_SOC_MIPI_DSI_SUPPORTED)
    list(APPEND srcs "${target}/mipi_dsi_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal)
