idf_build_get_property(target IDF_TARGET)

set(srcs "light_sleep_example_main.c"
         "gpio_wakeup.c"
         "timer_wakeup.c"
         "uart_wakeup.c")
set(includes ".")


idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_uart esp_driver_gpio esp_timer
                       INCLUDE_DIRS ${includes})
