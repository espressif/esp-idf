idf_component_register(SRCS "cmd_system.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES driver
                    REQUIRES console spi_flash)
