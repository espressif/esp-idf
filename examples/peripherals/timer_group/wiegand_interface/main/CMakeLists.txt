idf_component_register(SRCS "wiegand_example_main.c" "wiegand_io.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_gpio esp_driver_gptimer)
