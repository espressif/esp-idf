idf_component_register(SRCS "dac_continuous_example_main.c"
                            "dac_continuous_example_dma.c"
                            "dac_continuous_example_timer.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_dac esp_driver_gptimer)
