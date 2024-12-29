idf_build_get_property(target IDF_TARGET)

set(srcs "light_sleep_example_main.c"
         "gpio_wakeup.c"
         "timer_wakeup.c"
         "uart_wakeup.c")

set(priv_reqs esp_driver_uart esp_driver_gpio esp_timer)

set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

if(${target} IN_LIST TOUCH_ELEMENT_COMPATIBLE_TARGETS)
    list(APPEND srcs "touch_wakeup.c")
    list(APPEND priv_reqs touch_element)
endif()

if("${target}" STREQUAL "esp32p4")
    list(APPEND srcs "touch_sens_wakeup.c")
    list(APPEND priv_reqs esp_driver_touch_sens)
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_reqs}
                       INCLUDE_DIRS ".")
