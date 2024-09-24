idf_component_register(SRCS "deep_sleep_example_main.c"
                            "gpio_wakeup.c"
                            "ext_wakeup.c"
                            "touch_wakeup.c"
                       PRIV_REQUIRES driver nvs_flash ulp
                       INCLUDE_DIRS ".")
