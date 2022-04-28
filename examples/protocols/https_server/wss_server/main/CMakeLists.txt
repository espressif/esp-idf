idf_component_register(SRCS "wss_server_example.c" "keep_alive.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/servercert.pem"
                                   "certs/prvtkey.pem")
