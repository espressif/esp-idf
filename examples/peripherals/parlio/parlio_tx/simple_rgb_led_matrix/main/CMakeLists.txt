idf_component_register(SRCS "rgb_led_matrix_example_main.c" "lvgl_demo_ui.c"
                       PRIV_REQUIRES esp_driver_parlio esp_driver_gpio esp_driver_gptimer
                       INCLUDE_DIRS ".")
