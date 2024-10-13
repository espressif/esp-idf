set(srcs "test_app_main.c"
         "test_ppa.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_ppa esp_psram unity esp_mm
                       WHOLE_ARCHIVE)
