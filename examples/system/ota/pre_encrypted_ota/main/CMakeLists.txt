idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS "pre_encrypted_ota.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_http_client app_update esp_https_ota nvs_flash esp_netif esp_wifi
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem
                                   ${project_dir}/rsa_key/private.pem)

create_esp_enc_img(${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    ${project_dir}/rsa_key/private.pem ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}_secure.bin app)
