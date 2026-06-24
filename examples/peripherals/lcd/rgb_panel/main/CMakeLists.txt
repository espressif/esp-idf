idf_component_register(SRCS "rgb_lcd_example_main.c" "lvgl_demo_ui.c"
                       PRIV_REQUIRES esp_lcd esp_timer
                       INCLUDE_DIRS ".")
