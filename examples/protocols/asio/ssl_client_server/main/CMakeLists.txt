idf_component_register(SRCS "asio_ssl_main.cpp"
                    INCLUDE_DIRS "."
        EMBED_TXTFILES ca.crt server.key srv.crt)
