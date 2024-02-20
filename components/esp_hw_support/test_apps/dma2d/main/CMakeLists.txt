set(srcs "test_app_main.c"
         "test_dma2d.c"
         "dma2d_test_utils.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_mm
                       WHOLE_ARCHIVE)
