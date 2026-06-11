set(requires esp-tls nvs_flash esp_event esp_netif esp_http_client)

set(embed_txt certs/ca_cert.pem)

if(CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS)
    list(APPEND requires esp_secure_cert_mgr)
else()
    list(APPEND embed_txt certs/client_cert.pem certs/client_key.pem)
endif()

idf_component_register(SRCS "mutual_auth_example.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${requires}
                    EMBED_TXTFILES ${embed_txt})
