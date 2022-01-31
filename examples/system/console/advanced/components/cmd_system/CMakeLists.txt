idf_component_register(SRCS "cmd_system.c"
                    INCLUDE_DIRS .
                    REQUIRES console spi_flash driver)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
