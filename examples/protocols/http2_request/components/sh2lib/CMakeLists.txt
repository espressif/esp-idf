idf_component_register(SRCS "sh2lib.c"
                    INCLUDE_DIRS .
                    REQUIRES nghttp
                    PRIV_REQUIRES lwip esp-tls)
