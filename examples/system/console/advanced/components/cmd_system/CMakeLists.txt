idf_component_register(SRCS "cmd_system.c" "cmd_system_common.c"
                    INCLUDE_DIRS .
                    REQUIRES console spi_flash driver esp_driver_gpio)

if(CONFIG_SOC_DEEP_SLEEP_SUPPORTED OR CONFIG_SOC_LIGHT_SLEEP_SUPPORTED)
    target_sources(${COMPONENT_LIB} PRIVATE cmd_system_sleep.c)
endif()
