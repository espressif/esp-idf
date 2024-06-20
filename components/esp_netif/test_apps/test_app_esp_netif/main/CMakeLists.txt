if(CONFIG_ESP_NETIF_TCPIP_LWIP)
    set(srcs_test_stack esp_netif_test_lwip.c)
elseif(CONFIG_ESP_NETIF_LOOPBACK)
    set(srcs_test_stack esp_netif_test_loopback.c)
endif()

idf_component_register(SRCS esp_netif_test.c ${srcs_test_stack}
                   REQUIRES test_utils
                   INCLUDE_DIRS "."
                   PRIV_INCLUDE_DIRS "$ENV{IDF_PATH}/components/esp_netif/private_include" "."
                   PRIV_REQUIRES unity esp_netif nvs_flash esp_wifi)
