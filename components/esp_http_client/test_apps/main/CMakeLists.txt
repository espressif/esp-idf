idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                                      "../../lib/include"
                    PRIV_REQUIRES esp_http_client tcp_transport test_utils unity
                    WHOLE_ARCHIVE)
