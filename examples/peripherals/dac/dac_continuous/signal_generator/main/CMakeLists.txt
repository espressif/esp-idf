set(srcs "dac_conti_example_main.c"
        "dac_conti_example_dma.c"
        "dac_conti_example_timer.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
