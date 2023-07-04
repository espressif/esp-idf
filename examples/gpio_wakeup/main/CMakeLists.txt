set(srcs "gpio_wakeup_example_main.c"
         "power_manage/pm_configure.c"
         "gpio_wake_sleep/gpio_wakeup_sleep.c"
         "timer_wake_sleep/timer_wakeup_sleep.c")

set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

if(IDF_TARGET IN_LIST TOUCH_ELEMENT_COMPATIBLE_TARGETS)
    #list(APPEND srcs "touch_wakeup.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
