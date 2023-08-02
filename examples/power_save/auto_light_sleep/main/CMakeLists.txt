set(srcs "auto_light_sleep_example_main.c"
         "pm_config.c"
         "gpio_wakeup_sleep.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
