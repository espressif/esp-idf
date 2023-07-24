idf_component_register(SRCS "main.c" "tcp_server.c"
                    INCLUDE_DIRS ""
                    PRIV_REQUIRES protocol_examples_common nvs_flash usb mdns esp_ringbuf esp_timer esp_wifi driver)
