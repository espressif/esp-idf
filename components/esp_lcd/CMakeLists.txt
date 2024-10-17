idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "src/esp_lcd_common.c"
         "src/esp_lcd_panel_io.c"
         "src/esp_lcd_panel_nt35510.c"
         "src/esp_lcd_panel_ssd1306.c"
         "src/esp_lcd_panel_st7789.c"
         "src/esp_lcd_panel_ops.c")
set(includes "include" "interface")
set(priv_requires "esp_mm" "esp_psram" "esp_pm" "esp_driver_i2s")
set(public_requires "driver" "esp_driver_gpio" "esp_driver_i2c" "esp_driver_spi")

if(CONFIG_SOC_DMA2D_SUPPORTED)
    list(APPEND srcs "src/esp_async_fbcpy.c")
endif()

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "i2c/esp_lcd_panel_io_i2c_v1.c" "i2c/esp_lcd_panel_io_i2c_v2.c")
endif()

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "spi/esp_lcd_panel_io_spi.c")
endif()

if(CONFIG_SOC_I2S_SUPPORTS_LCD_CAMERA)
    list(APPEND srcs "i80/esp_lcd_panel_io_i2s.c")
endif()

if(CONFIG_SOC_LCDCAM_I80_LCD_SUPPORTED)
    list(APPEND srcs "i80/esp_lcd_panel_io_i80.c")
endif()

if(CONFIG_SOC_LCDCAM_RGB_LCD_SUPPORTED)
    list(APPEND includes "rgb/include")
    list(APPEND srcs "rgb/esp_lcd_panel_rgb.c")
endif()

if(CONFIG_SOC_MIPI_DSI_SUPPORTED)
    list(APPEND includes "dsi/include")
    list(APPEND srcs "dsi/esp_lcd_mipi_dsi_bus.c" "dsi/esp_lcd_panel_io_dbi.c" "dsi/esp_lcd_panel_dpi.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS "priv_include"
                       PRIV_REQUIRES ${priv_requires}
                       REQUIRES ${public_requires}
                       LDFRAGMENTS linker.lf)
