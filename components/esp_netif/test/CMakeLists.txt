idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS  "../private_include" "."
                    PRIV_REQUIRES cmock test_utils esp_netif nvs_flash driver)
