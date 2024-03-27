set(srcs "test_app_main.c"
         "test_i80_lcd_panel.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_lcd unity driver esp_driver_i2s
                       WHOLE_ARCHIVE)
