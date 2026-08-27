set(mock_transport_dir ${CMAKE_CURRENT_SOURCE_DIR}/../mock_transport)

idf_component_register(SRC_DIRS "." ${mock_transport_dir}
                    PRIV_INCLUDE_DIRS "."
                                      "../../lib/include"
                    PRIV_REQUIRES esp_http_client tcp_transport test_utils unity
                    INCLUDE_DIRS ${mock_transport_dir}
                    REQUIRES tcp_transport
                    WHOLE_ARCHIVE)
