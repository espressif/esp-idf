set(srcs "test_app_main.c"
         "test_gpio.c")

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "test_dedicated_gpio.c")
endif()

if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "test_sigma_delta_legacy.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
