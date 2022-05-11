set(srcs "src/esp_lcd_common.c"
         "src/esp_lcd_panel_io.c"
         "src/esp_lcd_panel_io_i2c.c"
         "src/esp_lcd_panel_io_spi.c"
         "src/esp_lcd_panel_nt35510.c"
         "src/esp_lcd_panel_ssd1306.c"
         "src/esp_lcd_panel_st7789.c"
         "src/esp_lcd_panel_ops.c")
set(includes "include" "interface")
set(priv_requires "driver")

if(CONFIG_SOC_I2S_LCD_I80_VARIANT)
    list(APPEND srcs "src/esp_lcd_panel_io_i2s.c")
endif()

if(CONFIG_SOC_LCDCAM_SUPPORTED)
    list(APPEND srcs "src/esp_lcd_panel_io_i80.c" "src/esp_lcd_rgb_panel.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

if(CONFIG_SPIRAM)
    idf_component_optional_requires(PRIVATE esp_psram)
endif()
