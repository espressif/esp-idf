idf_component_register(SRCS "main.c"
                            "spp_task.c"
                    PRIV_REQUIRES bt nvs_flash vfs
                    INCLUDE_DIRS ".")
