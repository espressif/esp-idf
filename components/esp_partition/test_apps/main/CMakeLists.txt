idf_component_register(SRCS test_part_app.c test_partition_ops.c
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES unity esp_partition spi_flash bootloader_support app_update esp_mspi esp_system
                       WHOLE_ARCHIVE
                      )
