set(srcs "test_app_main.c")

list(APPEND srcs "test_mmap_hw.c")


# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES test_utils spi_flash esp_mm
                       WHOLE_ARCHIVE)

idf_component_get_property(lib_name esp_mm COMPONENT_LIB)
# Add this to skip checking mapping to a paddr range that is enclosed by a previous mapped paddr range
target_compile_definitions(${lib_name} PRIVATE ESP_MMAP_TEST_ALLOW_MAP_TO_MAPPED_PADDR)
