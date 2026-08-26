idf_component_register(SRCS "test_psram_stack.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity nvs_flash esp_partition esp_hw_support esp_psram esp_pm)
