idf_component_register(SRCS "publish_test.c" "connect_test.c" "publish_connect_test.c"
                    INCLUDE_DIRS "."
                    REQUIRES mqtt nvs_flash console esp_netif)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
