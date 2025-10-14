idf_component_register(SRCS "bitscrambler_example_main.c"
                       PRIV_REQUIRES "esp_driver_bitscrambler"
                       INCLUDE_DIRS ".")

target_bitscrambler_add_src("example.bsasm")
