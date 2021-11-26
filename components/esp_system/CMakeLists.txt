idf_component_register(SRCS "panic.c" "system_api.c"
                    INCLUDE_DIRS include
                    PRIV_REQUIRES spi_flash app_update
                    LDFRAGMENTS "linker.lf")

add_subdirectory(port)
