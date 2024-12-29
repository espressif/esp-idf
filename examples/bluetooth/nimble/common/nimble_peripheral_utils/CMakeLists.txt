idf_component_register(SRCS "misc.c" "scli.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES bt console esp_driver_uart)
