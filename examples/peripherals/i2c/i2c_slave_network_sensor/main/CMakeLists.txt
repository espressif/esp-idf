set(srcs "i2c_slave_main.c")

idf_component_register(SRCS ${srcs}
                    PRIV_REQUIRES esp_http_client esp_wifi nvs_flash json esp_driver_i2c
                    INCLUDE_DIRS ".")
