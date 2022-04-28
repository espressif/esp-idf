idf_component_register(SRCS esp_log_impl.c strlcat.c
                       INCLUDE_DIRS include
                       REQUIRES esp_netif_linux esp_timer_linux freertos_linux esp_event_mock esp_netif log esp_common)
