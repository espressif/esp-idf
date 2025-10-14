set(srcs "deep_sleep_example_main.c"
                            "gpio_wakeup.c"
                            "ext_wakeup.c")
set(includes ".")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES nvs_flash ulp esp_driver_gpio
                       INCLUDE_DIRS ${includes})
