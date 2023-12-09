set(srcs "test_app_main.c")

if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "test_sdm.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_sdm
                       WHOLE_ARCHIVE)
