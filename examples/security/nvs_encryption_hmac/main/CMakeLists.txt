idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES nvs_flash spi_flash nvs_sec_provider
                       INCLUDE_DIRS ".")
