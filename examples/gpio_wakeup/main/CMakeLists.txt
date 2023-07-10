set(srcs "gpio_wakeup_example_main.c"
         "power_manage/pm_configure.c"
         "gpio_wake_sleep/gpio_wakeup_sleep.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
