idf_component_register(SRCS test_part_app.c
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES unity esp_partition spi_flash
                       WHOLE_ARCHIVE
                      )
