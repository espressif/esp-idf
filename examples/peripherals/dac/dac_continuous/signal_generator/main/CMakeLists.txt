set(srcs "dac_continuous_example_main.c"
        "dac_continuous_example_dma.c"
        "dac_continuous_example_timer.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
