set(srcs "test_app_main.c"
         "test_mipi_dsi_board.c"
         "test_mipi_dsi_panel.c")

if(CONFIG_LCD_DSI_ISR_CACHE_SAFE)
    list(APPEND srcs "test_mipi_dsi_iram.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_lcd unity esp_driver_ppa efuse
                       WHOLE_ARCHIVE)
