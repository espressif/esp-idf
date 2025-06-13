idf_component_register(SRCS "test_main.c" "test_pms.c"
                       INCLUDE_DIRS ""
                       PRIV_REQUIRES pms esp_psram
                       WHOLE_ARCHIVE)
