idf_component_register(SRCS "esp_eth_test.c"
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_eth esp_netif)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
