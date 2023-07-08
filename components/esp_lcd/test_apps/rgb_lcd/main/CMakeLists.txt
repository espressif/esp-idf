set(srcs "test_app_main.c"
         "test_rgb_panel.c")

if(CONFIG_SOC_LCD_SUPPORT_RGB_YUV_CONV)
    list(APPEND srcs "test_yuv_rgb_conv.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_lcd unity driver esp_timer spi_flash
                       WHOLE_ARCHIVE)
