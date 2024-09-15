set(srcs "soft_uart_main.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES soft_uart)
