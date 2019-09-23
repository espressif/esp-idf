idf_component_register(SRCS "src/custom_config.c"
                            "proto-c/custom_config.pb-c.c"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS proto-c
                    PRIV_REQUIRES protobuf-c)