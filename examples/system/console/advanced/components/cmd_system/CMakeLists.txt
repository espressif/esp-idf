idf_component_register(SRCS "cmd_system.c" "cmd_system_common.c" "cmd_system_sleep.c"
                    INCLUDE_DIRS .
                    REQUIRES console spi_flash driver)
