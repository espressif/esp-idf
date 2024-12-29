idf_component_register(SRCS "ir_nec_transceiver_main.c" "ir_nec_encoder.c"
                       PRIV_REQUIRES esp_driver_rmt
                       INCLUDE_DIRS ".")
