idf_component_register(SRCS "cmd_system.c" "cmd_system_common.c"
                    INCLUDE_DIRS .
                    REQUIRES console spi_flash driver)

if(NOT CONFIG_IDF_TARGET_ESP32H2)  # IDF-6268
    target_sources(${COMPONENT_LIB} PRIVATE cmd_system_sleep.c)
endif()
