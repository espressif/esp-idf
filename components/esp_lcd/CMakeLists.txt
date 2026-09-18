idf_build_get_property(target IDF_TARGET)

set(srcs "src/esp_lcd_panel_ops.c")
set(includes "include" "interface")
set(priv_includes "priv_include")
set(public_requires "esp_hal_lcd" "esp_hal_gpio")
set(priv_requires)

if(NOT ${target} STREQUAL "linux")
    list(APPEND srcs "src/esp_lcd_common.c"
                     "src/esp_lcd_panel_io.c"
                     "src/esp_lcd_panel_ssd1306.c"
                     "src/esp_lcd_panel_st7789.c")
    list(APPEND priv_requires "esp_mm" "esp_psram" "esp_pm" "esp_driver_i2s" "esp_driver_dma")
    list(APPEND public_requires "esp_driver_gpio" "esp_driver_i2c" "esp_driver_spi" "esp_driver_parlio")
endif()

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "i2c/esp_lcd_panel_io_i2c.c")
endif()

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "spi/esp_lcd_panel_io_spi.c")
endif()

if(CONFIG_SOC_I2S_I80_LCD_SUPPORTED)
    list(APPEND srcs "i80/esp_lcd_panel_io_i2s.c")
endif()

if(CONFIG_SOC_PARLIO_LCD_SUPPORTED)
    list(APPEND srcs "parl/esp_lcd_panel_io_parl.c")
endif()

if(CONFIG_SOC_LCDCAM_I80_LCD_SUPPORTED)
    list(APPEND srcs "i80/esp_lcd_panel_io_i80.c")
    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "i80/${target}/lcd_retention.c")
    endif()
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
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       REQUIRES ${public_requires}
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # GCC -fanalyzer false positives on early-return cleanup paths (see build logs with -Werror=analyzer-malloc-leak).
    set_source_files_properties("src/esp_lcd_panel_ssd1306.c"
                                "src/esp_lcd_panel_st7789.c"
                                PROPERTIES COMPILE_FLAGS "-Wno-analyzer-malloc-leak")
    if(CONFIG_SOC_I2C_SUPPORTED)
        set_source_files_properties("i2c/esp_lcd_panel_io_i2c.c"
                                    PROPERTIES COMPILE_FLAGS "-Wno-analyzer-malloc-leak")
    endif()
endif()
