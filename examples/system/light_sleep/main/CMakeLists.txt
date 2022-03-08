set(srcs "light_sleep_example_main.c"
         "gpio_wakeup.c"
         "timer_wakeup.c"
         "uart_wakeup.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
