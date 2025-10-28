idf_component_register(SRCS "mqtt_app.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash esp_event esp_netif mqtt)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
