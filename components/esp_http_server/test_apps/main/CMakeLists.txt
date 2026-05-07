idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "." "../../src" "../../src/port/esp32"
                    PRIV_REQUIRES esp_http_server test_utils unity esp_timer)
