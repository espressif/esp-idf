idf_component_register(SRCS "test_app_main.c" "test_attr.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_mm esp_psram
                       WHOLE_ARCHIVE)
