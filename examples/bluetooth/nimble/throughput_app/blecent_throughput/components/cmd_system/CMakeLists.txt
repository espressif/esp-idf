idf_component_register(SRCS "cmd_system.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES driver
                    REQUIRES console spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
