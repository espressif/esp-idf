idf_component_register(SRCS "app_main.c"
                       PRIV_REQUIRES esp_driver_gptimer esp_partition nvs_flash spi_flash
                       INCLUDE_DIRS "")
