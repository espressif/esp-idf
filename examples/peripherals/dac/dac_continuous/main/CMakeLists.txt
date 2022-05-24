set(srcs "dac_continuous_main.c"
        "dac_continuous_dma.c"
        "dac_continuous_timer.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
