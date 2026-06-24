set(srcs "tee_srv_sec_str.c"
         "app_main.c")

if(CONFIG_SOC_WIFI_SUPPORTED)
    list(APPEND srcs "tee_cmd_wifi.c" "tee_srv_ota.c")
endif()

if(CONFIG_SECURE_TEE_ATTESTATION)
    list(APPEND srcs "tee_srv_att.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES app_update console esp_event esp_http_client
                                     esp_https_ota esp_wifi mbedtls nvs_flash)
