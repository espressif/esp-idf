set(srcs "external_power_control_example_main.c"
         "pm_config.c"
         "gpio_control/gpio_control.c"
         "uart_control/uart_control.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
