set(srcs "light_sleep_example_main.c"
         "gpio_wakeup.c"
         "timer_wakeup.c"
         "uart_wakeup.c")

set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

if(IDF_TARGET IN_LIST TOUCH_ELEMENT_COMPATIBLE_TARGETS)
    list(APPEND srcs "touch_wakeup.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
