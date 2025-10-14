idf_component_register(SRCS "advanced_rgb_led_matrix_example_main.c" "lvgl_demo_ui.c"
                       PRIV_REQUIRES esp_driver_parlio esp_timer
                       INCLUDE_DIRS ".")
