idf_component_register(SRCS "asio/asio/src/asio.cpp"
                            "asio/asio/src/asio_ssl.cpp"
                            "port/src/esp_asio_openssl_stubs.c"
                    INCLUDE_DIRS "asio/asio/include" "port/include"
                    REQUIRES lwip openssl)
