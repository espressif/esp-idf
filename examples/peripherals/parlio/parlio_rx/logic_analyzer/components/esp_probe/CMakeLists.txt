set(src "hw_impl/esp_probe_impl_parlio.c" "esp_probe.c"
        "stream/flash_fatfs.c" "stream/file_stream.c")

if(CONFIG_SOC_WIFI_SUPPORTED OR CONFIG_SOC_EMAC_SUPPORTED)
    list(APPEND src "stream/tcp_stream.c")
endif()

idf_component_register(SRCS             ${src}
                       INCLUDE_DIRS     "./include"
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES    fatfs esp_partition esp_netif
                                        lwip esp_driver_parlio)
