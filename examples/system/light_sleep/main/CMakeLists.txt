idf_build_get_property(target IDF_TARGET)

set(srcs "light_sleep_example_main.c"
         "gpio_wakeup.c"
         "timer_wakeup.c"
         "uart_wakeup.c")
set(includes ".")

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED AND CONFIG_SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP)
    list(APPEND srcs "touch_sens_wakeup.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_uart esp_driver_gpio esp_timer esp_driver_touch_sens
                       INCLUDE_DIRS ${includes})
