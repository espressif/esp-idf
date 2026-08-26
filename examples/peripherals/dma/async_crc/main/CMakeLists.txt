idf_component_register(SRCS "async_crc_example_main.c"
                            "cmd_crc.c"
                       PRIV_REQUIRES console esp_driver_dma
                       INCLUDE_DIRS ".")
