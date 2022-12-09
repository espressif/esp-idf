idf_component_register(SRCS "connect.c" "stdin_out.c" "addr_from_stdin.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_netif
                    )
