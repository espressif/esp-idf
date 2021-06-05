idf_component_register(SRCS "advanced_https_ota_example.c" "ble_helper/bluedroid_gatts.c" "ble_helper/nimble_gatts.c"
                    "ble_helper/ble_api.c"
                    INCLUDE_DIRS "." "./ble_helper/include/"
                    # Embed the server root certificate into the final binary
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)
