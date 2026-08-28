set(srcs "test_app_main.c"
         "test_phy_multiple_init_data.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_phy esp_wifi esp_event nvs_flash spi_flash
                       WHOLE_ARCHIVE)
