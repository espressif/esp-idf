idf_component_register(SRCS "test_app_main.c" "test_panic.c"
                       INCLUDE_DIRS "include"
                       REQUIRES spi_flash esp_system esp_partition)
