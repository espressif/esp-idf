idf_component_register(SRC_DIRS .
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES cmock test_utils esp_eth esp_http_client
                       EMBED_TXTFILES dl_espressif_com_root_cert.pem)
