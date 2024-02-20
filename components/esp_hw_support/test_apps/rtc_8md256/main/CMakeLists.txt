set(srcs "test_app_main.c"
         "test_rtc_8md256.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_uart
                       WHOLE_ARCHIVE)
